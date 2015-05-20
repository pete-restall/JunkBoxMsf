#include <xc.h>

#include "../Config.h"
#include "../JunkBoxMsf.h"
#include "../Mcu.h"

#include "Lcd.h"

#define ROUND_FLOAT_TO_NEAREST_UINT(x) ((unsigned int) ((x) + 0.5))
#define LCD_PWM_FREQUENCY_HZ 200000.0
#define LCD_PWM_PERIOD ROUND_FLOAT_TO_NEAREST_UINT(MCU_INSTRUCTION_FREQUENCY_HZ / LCD_PWM_FREQUENCY_HZ) - 1
#define LCD_PWM_LOW_POWER_PERIOD \
	ROUND_FLOAT_TO_NEAREST_UINT(MCU_LOW_POWER_INSTRUCTION_FREQUENCY_HZ / LCD_PWM_FREQUENCY_HZ) - 1

#define LCD_MAXIMUM_CONTRAST 0
#define LCD_MINIMUM_CONTRAST (2 * (PTPER + 1))
#define LCD_CONTRAST_STEPS 16
#define LCD_CONTRAST_STEP ((LCD_MINIMUM_CONTRAST - LCD_MAXIMUM_CONTRAST) / LCD_CONTRAST_STEPS)

#define PWMCON1_PMOD_INDEPENDENT 1

#define LCD_CMD_CLEAR_DISPLAY 0x01

#define LCD_CMD_RETURN_HOME 0x02

#define LCD_CMD_ENTRY_MODE 0x04
#define LCD_CMD_ENTRY_MODE_BIT_INCREMENT 0x02
#define LCD_CMD_ENTRY_MODE_BIT_SHIFT 0x01

#define LCD_CMD_DISPLAY 0x08
#define LCD_CMD_DISPLAY_BIT_ON 0x04
#define LCD_CMD_DISPLAY_BIT_CURSOR 0x02
#define LCD_CMD_DISPLAY_BIT_BLINK 0x01

#define LCD_CMD_DISPLAY_SHIFT 0x10
#define LCD_CMD_DISPLAY_SHIFT_BIT_RIGHT 0x04
#define LCD_CMD_DISPLAY_SHIFT_BIT_WHOLE_SCREEN 0x08

#define LCD_CMD_FUNCTION 0x20
#define LCD_CMD_FUNCTION_BIT_BYTE 0x10
#define LCD_CMD_FUNCTION_BIT_TWO_LINES 0x08
#define LCD_CMD_FUNCTION_BIT_FIVE_BY_TEN 0x04

#define LCD_CMD_SET_DDRAM_ADDRESS 0x80

#define LCD_NUMBER_OF_LINES 2
#define LCD_LINE_WIDTH 0x40
#define LCD_BUSY_FLAG 0x80

extern unsigned char lcdReadBusyFlags(void);
extern void lcdWriteRegister(unsigned char data);
extern void lcdWriteMemory(unsigned char data);

static void pwmInitialise(void);
static void lcdInitialisationSequence(void);
static UpdateStatus lcdStateIdle(void);
static void lcdInitialiseNibbleMode(void);
static void lcdSetByteModeWithNibbleHardware(void);
static void lcdPulseDataLines(unsigned char db7, unsigned char db6, unsigned char db5, unsigned char db4);
static void lcdSetNibbleModeWhenInByteMode(void);
static void lcdWaitUntilNotBusy(void);
static void lcdWriteRegisterWithNextStateAndCallback(unsigned char value, LcdState nextState, LcdCallback callback);
static UpdateStatus lcdStateWaitUntilNotBusy(void);
static UpdateStatus lcdStateMakeIdleCallback(void);
static UpdateStatus lcdStateWaitUntilNotBusyAndAddressMatches(void);
static UpdateStatus lcdStatePutNextStringCharacter(void);
static UpdateStatus lcdStateStopShifting(void);

LcdState lcdUpdate;
unsigned int lcdLowPowerDelayScale;
static LcdCallback lcdCallback;
static LcdState lcdNextState;
static const char *lcdStringBuffer;
static unsigned int lcdStringPaddedToLength;
static unsigned char lcdExpectedAddress;

void lcdInitialise(void)
{
	LATDbits.LATD0 = 0;
	LATDbits.LATD1 = 0;
	LATE = 0;

	TRISE = 0;
	TRISDbits.TRISD0 = 0;
	TRISDbits.TRISD1 = 0;

	lcdLowPowerDelayScale = 0;
	lcdOn();
	lcdUpdate = lcdStateIdle;
	lcdInitialisationSequence();
}

void lcdOn(void)
{
	pwmInitialise();
	lcdSetContrast(config.lcd.contrast);
}

static void pwmInitialise(void)
{
	PMD1bits.PWMMD = 0;
	nop2();

	PTCONbits.PTEN = 0;
	PWMCON1 = 0;
	PWMCON1bits.PMOD3 = PWMCON1_PMOD_INDEPENDENT;
	PWMCON1bits.PEN3H = 1;
	PTMR = 0;
	PTPER = lcdLowPowerDelayScale ? LCD_PWM_LOW_POWER_PERIOD : LCD_PWM_PERIOD;
	PDC3 = 0;
	PTCONbits.PTEN = 1;
}

static UpdateStatus lcdStateIdle(void)
{
	return UpdateStatus_Idle;
}

static void lcdInitialisationSequence(void)
{
	lcdInitialiseNibbleMode();
	lcdWriteRegister(LCD_CMD_FUNCTION | LCD_CMD_FUNCTION_BIT_TWO_LINES);
	lcdWaitUntilNotBusy();
	lcdWriteRegister(LCD_CMD_DISPLAY);
	lcdWaitUntilNotBusy();
	lcdWriteRegister(LCD_CMD_CLEAR_DISPLAY);
	lcdWaitUntilNotBusy();
	lcdWriteRegister(LCD_CMD_RETURN_HOME);
	lcdWaitUntilNotBusy();
	lcdWriteRegister(LCD_CMD_ENTRY_MODE | LCD_CMD_ENTRY_MODE_BIT_INCREMENT);
	lcdWaitUntilNotBusy();
	lcdWriteRegister(LCD_CMD_DISPLAY_SHIFT);
	lcdWaitUntilNotBusy();
}

static void lcdInitialiseNibbleMode(void)
{
	/* See page 46 of the HD44780 datasheet for this sequence */

	mcuWaitMilliseconds(45);
	lcdSetByteModeWithNibbleHardware();
	mcuWaitMilliseconds(5);
	lcdSetByteModeWithNibbleHardware();
	mcuWaitMilliseconds(1);
	lcdSetByteModeWithNibbleHardware();
	mcuWaitMilliseconds(1);
	lcdSetNibbleModeWhenInByteMode();
	mcuWaitMilliseconds(1);
}

static void lcdSetByteModeWithNibbleHardware(void)
{
	lcdPulseDataLines(0, 0, 1, 1);
}

static void lcdPulseDataLines(unsigned char db7, unsigned char db6, unsigned char db5, unsigned char db4)
{
	LATDbits.LATD0 = 0;
	LATEbits.LATE4 = 0;
	LATEbits.LATE0 = db4;
	LATEbits.LATE1 = db5;
	LATEbits.LATE2 = db6;
	LATEbits.LATE3 = db7;
	TRISE &= 0xf0;
	mcuWaitMicroseconds(1);
	LATDbits.LATD0 = 1;
	mcuWaitMicroseconds(2);
	LATDbits.LATD0 = 0;
	mcuWaitMicroseconds(1);
}

static void lcdSetNibbleModeWhenInByteMode(void)
{
	lcdPulseDataLines(0, 0, 1, 0);
}

void lcdEnable(void)
{
	lcdWaitUntilNotBusy();
	lcdWriteRegister(LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_BIT_ON);
	lcdWaitUntilNotBusy();
}

static void lcdWaitUntilNotBusy(void)
{
	while (lcdReadBusyFlags() & LCD_BUSY_FLAG);
	mcuWaitMicroseconds(10);
}

int lcdSetCursorPosition(unsigned char x, unsigned char y, LcdCallback callback)
{
	if (lcdUpdate != lcdStateIdle)
		return 0;

	lcdExpectedAddress = ((y % LCD_NUMBER_OF_LINES) * LCD_LINE_WIDTH + (x % LCD_LINE_WIDTH));

	lcdWriteRegisterWithNextStateAndCallback(
		LCD_CMD_SET_DDRAM_ADDRESS | lcdExpectedAddress,
		lcdStateMakeIdleCallback,
		callback);

	lcdUpdate = lcdStateWaitUntilNotBusyAndAddressMatches;

	return 1;
}

static void lcdWriteRegisterWithNextStateAndCallback(unsigned char value, LcdState nextState, LcdCallback callback)
{
	lcdWriteRegister(value);
	lcdCallback = callback;
	lcdNextState = nextState;
	lcdUpdate = lcdStateWaitUntilNotBusy;
}

static UpdateStatus lcdStateWaitUntilNotBusy(void)
{
	if (lcdReadBusyFlags() & LCD_BUSY_FLAG)
		return UpdateStatus_MoreWorkQueued;

	lcdUpdate = lcdNextState;
	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus lcdStateMakeIdleCallback(void)
{
	lcdUpdate = lcdStateIdle;
	if (lcdCallback)
		lcdCallback();

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus lcdStateWaitUntilNotBusyAndAddressMatches(void)
{
	if (lcdReadBusyFlags() == lcdExpectedAddress)
		lcdUpdate = lcdNextState;

	return UpdateStatus_MoreWorkQueued;
}

int lcdPutStringPadded(const char *str, unsigned int length, LcdCallback callback)
{
	if (lcdUpdate != lcdStateIdle)
		return 0;

	lcdCallback = callback;
	lcdStringBuffer = str;
	lcdStringPaddedToLength = length;
	lcdStatePutNextStringCharacter();
	return 1;
}

static UpdateStatus lcdStatePutNextStringCharacter(void)
{
	if (*lcdStringBuffer)
	{
		lcdWriteMemory(*lcdStringBuffer);
		lcdStringBuffer++;
		lcdNextState = lcdStatePutNextStringCharacter;
	}
	else
	{
		if (lcdStringPaddedToLength > 0)
			lcdWriteMemory(' ');

		lcdNextState = lcdStringPaddedToLength > 1
			? lcdStatePutNextStringCharacter
			: lcdStateMakeIdleCallback;
	}

	if (lcdStringPaddedToLength > 0)
		lcdStringPaddedToLength--;

	lcdUpdate = lcdStateWaitUntilNotBusy;
	return UpdateStatus_MoreWorkQueued;
}

int lcdShiftRight(LcdCallback callback)
{
	if (lcdUpdate != lcdStateIdle)
		return 0;

	lcdWriteRegisterWithNextStateAndCallback(
		LCD_CMD_DISPLAY_SHIFT | LCD_CMD_DISPLAY_SHIFT_BIT_WHOLE_SCREEN | LCD_CMD_DISPLAY_SHIFT_BIT_RIGHT,
		lcdStateStopShifting,
		callback);

	return 1;
}

static UpdateStatus lcdStateStopShifting(void)
{
	lcdWriteRegister(LCD_CMD_DISPLAY_SHIFT);
	lcdNextState = lcdStateMakeIdleCallback;
	lcdUpdate = lcdStateWaitUntilNotBusy;
	return UpdateStatus_MoreWorkQueued;
}

int lcdShiftLeft(LcdCallback callback)
{
	if (lcdUpdate != lcdStateIdle)
		return 0;

	lcdWriteRegisterWithNextStateAndCallback(
		LCD_CMD_DISPLAY_SHIFT | LCD_CMD_DISPLAY_SHIFT_BIT_WHOLE_SCREEN,
		lcdStateStopShifting,
		callback);

	return 1;
}

void lcdBacklightOn(void)
{
	LATDbits.LATD1 = 1;
}

void lcdOff(void)
{
	lcdBacklightOff();

	LATEbits.LATE5 = 1;
	PTCONbits.PTEN = 0;
	PMD1bits.PWMMD = 1;
	nop2();
	TRISEbits.TRISE5 = 0;
}

void lcdBacklightOff(void)
{
	LATDbits.LATD1 = 0;
}

int lcdSetContrast(unsigned int step)
{
	unsigned int newDutyCycle = step * LCD_CONTRAST_STEP;
	if (newDutyCycle <= LCD_MAXIMUM_CONTRAST)
	{
		PDC3 = LCD_MAXIMUM_CONTRAST;
		return -1;
	}
	else if (newDutyCycle >= LCD_MINIMUM_CONTRAST)
	{
		PDC3 = LCD_MINIMUM_CONTRAST;
		return 1;
	}

	PDC3 = newDutyCycle;
	return 0;
}

void lcdLowPowerModeEnable(void)
{
	lcdLowPowerDelayScale = 16;
	PTPER = LCD_PWM_LOW_POWER_PERIOD;
	lcdSetContrast(config.lcd.contrast);
}

void lcdLowPowerModeDisable(void)
{
	lcdLowPowerDelayScale = 0;
	PTPER = LCD_PWM_PERIOD;
	lcdSetContrast(config.lcd.contrast);
}
