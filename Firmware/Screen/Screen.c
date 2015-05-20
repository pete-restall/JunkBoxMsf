#include <xc.h>

#include "../Clock.h"
#include "../JunkBoxMsf.h"
#include "../Screen.h"
#include "Lcd.h"

#define SCREEN_WIDTH 16
#define SCREEN_SHIFT_DELAY_TICKS MILLISECONDS_TO_TICKS(120)

static UpdateStatus screenStateIdle(void);
static UpdateStatus screenStateShiftLcdLeft(void);
static UpdateStatus screenStateShift(int (*shifter)(LcdCallback));
static UpdateStatus screenStateShiftLcdRight(void);
static UpdateStatus screenStateUpdateLcd(void);
static void screenStatePutFirstLine(void);
static void screenStatePutSecondLine(void);
static void screenStatePutSecondLineAndMakeIdleCallback(void);
static void screenStateMakeIdleCallback(void);
static int screenPutStringAt(unsigned int x, unsigned int y, const char *str, ScreenCallback callback);
static void screenStatePutFirstLineOnly(void);
static void screenStatePutSecondLineOnly(void);
static void screenStatePutOtherFirstLine(void);
static void screenStatePutOtherSecondLine(void);

ScreenState screenUpdate;
static unsigned int screenCurrentStartX;
static unsigned int screenOtherStartX;
static ScreenCallback screenCallback;
static const char *screenLines[2];
static unsigned int screenShiftCounter;
static unsigned int screenShiftStartTime;

void screenInitialise(void)
{
	screenUpdate = screenStateIdle;
	screenCurrentStartX = 0;
	screenOtherStartX = SCREEN_WIDTH;
	lcdInitialise();
	lcdEnable();
}

static UpdateStatus screenStateIdle(void)
{
	return UpdateStatus_Idle;
}

void screenEnable(void)
{
}

int screenPutStringsOnLeftBothLines(const char *firstLine, const char *secondLine, ScreenCallback callback)
{
	if (screenCurrentStartX == 0)
		return screenPutStringsOnBothLines(firstLine, secondLine, callback);

	return screenPutStringsOnOtherBothLines(firstLine, secondLine, callback);
}

int screenPutStringsOnBothLines(const char *firstLine, const char *secondLine, ScreenCallback callback)
{
	if (screenUpdate != screenStateIdle)
		return 0;

	screenLines[SCREEN_FIRST_LINE] = firstLine;
	screenLines[SCREEN_SECOND_LINE] = secondLine;
	screenCallback = callback;
	screenUpdate = screenStateUpdateLcd;
	return lcdSetCursorPosition(screenCurrentStartX, 0, screenStatePutFirstLine);
}

static UpdateStatus screenStateUpdateLcd(void)
{
	UpdateStatus status = lcdUpdate();
	if (status != UpdateStatus_Idle)
		return status;

	screenUpdate = screenStateIdle;
	return UpdateStatus_Idle;
}

static void screenStatePutFirstLine(void)
{
	lcdPutStringPadded(screenLines[SCREEN_FIRST_LINE], SCREEN_WIDTH, screenStatePutSecondLine);
}

static void screenStatePutSecondLine(void)
{
	lcdSetCursorPosition(screenCurrentStartX, 1, screenStatePutSecondLineAndMakeIdleCallback);
}

static void screenStatePutSecondLineAndMakeIdleCallback(void)
{
	lcdPutStringPadded(screenLines[SCREEN_SECOND_LINE], SCREEN_WIDTH, screenStateMakeIdleCallback);
}

static void screenStateMakeIdleCallback(void)
{
	screenUpdate = screenStateIdle;
	if (screenCallback)
		screenCallback();
}

int screenPutStringsOnOtherBothLines(const char *firstLine, const char *secondLine, ScreenCallback callback)
{
	if (screenUpdate != screenStateIdle)
		return 0;

	screenLines[SCREEN_FIRST_LINE] = firstLine;
	screenLines[SCREEN_SECOND_LINE] = secondLine;
	screenCallback = callback;
	screenUpdate = screenStateUpdateLcd;
	return lcdSetCursorPosition(screenOtherStartX, 0, screenStatePutOtherFirstLine);
}

static void screenStatePutOtherFirstLine(void)
{
	lcdPutStringPadded(screenLines[SCREEN_FIRST_LINE], SCREEN_WIDTH, screenStatePutOtherSecondLine);
}

static void screenStatePutOtherSecondLine(void)
{
	lcdSetCursorPosition(screenOtherStartX, 1, screenStatePutSecondLineAndMakeIdleCallback);
}

int screenPutStringsOnRightBothLines(const char *firstLine, const char *secondLine, ScreenCallback callback)
{
	if (screenCurrentStartX != 0)
		return screenPutStringsOnBothLines(firstLine, secondLine, callback);

	return screenPutStringsOnOtherBothLines(firstLine, secondLine, callback);
}

int screenPutStringOnLeftLine(unsigned int line, const char *str, ScreenCallback callback)
{
	if (screenCurrentStartX == 0)
		return screenPutStringOnLine(line, str, callback);

	return screenPutStringOnOtherLine(line, str, callback);
}

int screenPutStringOnLine(unsigned int line, const char *str, ScreenCallback callback)
{
	return screenPutStringAt(screenCurrentStartX, line, str, callback);
}

static int screenPutStringAt(unsigned int x, unsigned int y, const char *str, ScreenCallback callback)
{
	if (screenUpdate != screenStateIdle)
		return 0;

	y &= 0x0001;
	screenCallback = callback;
	screenUpdate = screenStateUpdateLcd;
	if (y == 0)
	{
		screenLines[SCREEN_FIRST_LINE] = str;
		return lcdSetCursorPosition(x, y, screenStatePutFirstLineOnly);
	}
	else
	{
		screenLines[SCREEN_SECOND_LINE] = str;
		return lcdSetCursorPosition(x, y, screenStatePutSecondLineOnly);
	}
}

static void screenStatePutFirstLineOnly(void)
{
	lcdPutStringPadded(screenLines[SCREEN_FIRST_LINE], SCREEN_WIDTH, screenStateMakeIdleCallback);
}

static void screenStatePutSecondLineOnly(void)
{
	lcdPutStringPadded(screenLines[SCREEN_SECOND_LINE], SCREEN_WIDTH, screenStateMakeIdleCallback);
}

int screenPutStringOnRightLine(unsigned int line, const char *str, ScreenCallback callback)
{
	if (screenCurrentStartX != 0)
		return screenPutStringOnLine(line, str, callback);

	return screenPutStringOnOtherLine(line, str, callback);
}

int screenPutStringOnOtherLine(unsigned int line, const char *str, ScreenCallback callback)
{
	return screenPutStringAt(screenOtherStartX, line, str, callback);
}

int screenTransitionToLeft(ScreenCallback callback)
{
	if (screenCurrentStartX == 0)
	{
		if (callback)
		{
			if (screenUpdate != screenStateIdle)
				return 0;

			callback();
		}

		return 1;
	}

	return screenTransitionToOther(callback);
}

int screenTransitionToOther(ScreenCallback callback)
{
	unsigned int exchanged;
	if (screenUpdate != screenStateIdle)
		return 0;

	exchanged = screenCurrentStartX;
	screenCurrentStartX = screenOtherStartX;
	screenOtherStartX = exchanged;
	screenShiftCounter = SCREEN_WIDTH;
	screenShiftStartTime = NOW_IN_TICKS;
	screenCallback = callback;
	screenUpdate = exchanged == 0 ? screenStateShiftLcdLeft : screenStateShiftLcdRight;
	return 1;
}

static UpdateStatus screenStateShiftLcdLeft(void)
{
	return screenStateShift(lcdShiftLeft);
}

static UpdateStatus screenStateShift(int (*shifter)(LcdCallback))
{
	if (lcdUpdate() != UpdateStatus_Idle)
		return UpdateStatus_MoreWorkQueued;

	if (TICKS_ELAPSED_SINCE(screenShiftStartTime) >= SCREEN_SHIFT_DELAY_TICKS)
	{
		screenShiftStartTime = NOW_IN_TICKS;
		if (screenShiftCounter--)
			shifter(NO_CALLBACK);
		else
			screenUpdate = screenStateUpdateLcd;
	}

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus screenStateShiftLcdRight(void)
{
	return screenStateShift(lcdShiftRight);
}

int screenTransitionToRight(ScreenCallback callback)
{
	if (screenCurrentStartX != 0)
	{
		if (callback)
		{
			if (screenUpdate != screenStateIdle)
				return 0;

			callback();
		}

		return 1;
	}

	return screenTransitionToOther(callback);
}
