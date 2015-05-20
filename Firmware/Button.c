#include <xc.h>

#include "Button.h"
#include "Screen/Lcd.h"
#include "Clock.h"
#include "Config.h"

#define BUTTON_SAMPLE_MILLISECONDS 2

#define BUTTON_LONG_PRESS_COUNT (400 / BUTTON_SAMPLE_MILLISECONDS)
#define BUTTON_SHORT_PRESS_COUNT (0 / BUTTON_SAMPLE_MILLISECONDS)

static unsigned long buttonReleasedTimestamp;
static ButtonPressCallback shortButtonPressCallback;
static ButtonPressCallback longButtonPressCallback;
ButtonState buttonUpdate;

static unsigned int lastSampleTime;
static unsigned int state;
static unsigned int pressCounter;
static unsigned int totalPressCount;
static unsigned int longButtonPressCallbackCalled;

static unsigned int pressCount(void);
static UpdateStatus buttonStatePressed(void);
static UpdateStatus buttonStateReleased(void);

void buttonInitialise(void)
{
	IFS0bits.CNIF = 0;
	IPC3bits.CNIP = 0;
	CNEN1bits.CN7IE = 1;
	IEC0bits.CNIE = 1;

	buttonReleasedTimestamp = 0;
	shortButtonPressCallback = 0;
	longButtonPressCallback = 0;
	buttonUpdate = buttonStateReleased;

	lastSampleTime = 0;
	state = 0;
	pressCounter = 0;
	totalPressCount = 0;
	longButtonPressCallbackCalled = 0;
}

static UpdateStatus buttonStateReleased(void)
{
	unsigned long secondsSinceLastButtonRelease = UPTIME_SECONDS_SINCE(buttonReleasedTimestamp);

	if (pressCount())
	{
		buttonUpdate = buttonStatePressed;
		return UpdateStatus_MoreWorkQueued;
	}

	if (secondsSinceLastButtonRelease >= config.lcd.backlightSecondsAfterButtonPress)
		lcdBacklightOff();

	if (config.lcd.lowPowerTimeoutAfterButtonPress &&
	    secondsSinceLastButtonRelease >= config.lcd.lowPowerTimeoutAfterButtonPress)
	{
		lcdOff();
	}

	return IFS0bits.CNIF ? UpdateStatus_MoreWorkQueued : UpdateStatus_Idle;
}

static unsigned int pressCount(void)
{
	if (TICKS_ELAPSED_SINCE(lastSampleTime) < MILLISECONDS_TO_TICKS(BUTTON_SAMPLE_MILLISECONDS))
		return pressCounter;

	lastSampleTime = NOW_IN_TICKS;
	state = 0xe000 | (state << 1) | (PORTBbits.RB5 ? 0 : 1);
	if (state == 0xf000 || (state == 0xe000 && pressCounter))
	{
		if (pressCounter != 0xffff)
			pressCounter++;
	}
	else
	{
		pressCounter = 0;
		if (state == 0xffff)
			IFS0bits.CNIF = 0;
	}

	return pressCounter;
}

static UpdateStatus buttonStatePressed(void)
{
	unsigned int currentPressCount = pressCount();

	lcdOn();
	if (config.lcd.backlightSecondsAfterButtonPress > 0)
		lcdBacklightOn();

	if (currentPressCount == 0)
	{
		if (totalPressCount >= BUTTON_SHORT_PRESS_COUNT && totalPressCount < BUTTON_LONG_PRESS_COUNT)
		{
			if (shortButtonPressCallback)
				shortButtonPressCallback();
		}

		longButtonPressCallbackCalled = 0;
		totalPressCount = 0;
		buttonUpdate = buttonStateReleased;
		buttonReleasedTimestamp = UPTIME_NOW;
		return UpdateStatus_MoreWorkQueued;
	}

	totalPressCount = currentPressCount;
	if (totalPressCount >= BUTTON_LONG_PRESS_COUNT && !longButtonPressCallbackCalled)
	{
		longButtonPressCallbackCalled = ~0;
		if (longButtonPressCallback)
			longButtonPressCallback();
	}

	return UpdateStatus_MoreWorkQueued;
}

void buttonOnShortPress(ButtonPressCallback callback)
{
	shortButtonPressCallback = callback;
}

void buttonOnLongPress(ButtonPressCallback callback)
{
	longButtonPressCallback = callback;
}
