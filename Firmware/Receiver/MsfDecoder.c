#include <xc.h>
#include <string.h>

#include "../Clock.h"
#include "../Config.h"
#include "../Menu.h"
#include "../Screen.h"

#include "MsfDecoder.h"

#define BCD_PAIR_HIGH_DIGIT_CHAR_OR_QUESTION(x) BCD_PAIR_LOW_DIGIT_CHAR_OR_QUESTION((x) >> 4)
#define BCD_PAIR_LOW_DIGIT_CHAR_OR_QUESTION(x) DIGIT_CHAR_OR_QUESTION('0' + ((x) & 0x0f))
#define DIGIT_CHAR_OR_QUESTION(x) ((x) >= '0' && (x) <= '9' ? (x) : '?')
#define BCD_PAIR_TO_BINARY(x) ((((x) >> 4) & 0x0f) * 10 + ((x) & 0x0f))
#define IS_VALID_BCD_DIGIT_CHAR(x) ((x) != '?')

#define IS_PARITY_ODD(x) ((x) & 0x01)
#define IS_POSSIBLY_VALID(x) ((possiblyValidDateTimeParts & (x)) == (x))
#define IS_VALID(x) ((invalidDateTimeParts & (x)) == 0)

#define SET_DATE_YY_DIGIT1(x) date[2] = (x)
#define SET_DATE_YY_DIGIT2(x) date[3] = (x)
#define SET_DATE_MM_DIGIT1(x) date[5] = (x)
#define SET_DATE_MM_DIGIT2(x) date[6] = (x)
#define SET_DATE_DD_DIGIT1(x) date[8] = (x)
#define SET_DATE_DD_DIGIT2(x) date[9] = (x)

#define SET_TIME_HH_DIGIT1(x) time[0] = (x)
#define SET_TIME_HH_DIGIT2(x) time[1] = (x)
#define SET_TIME_MM_DIGIT1(x) time[3] = (x)
#define SET_TIME_MM_DIGIT2(x) time[4] = (x)
#define SET_TIME_SS_DIGIT1(x) time[6] = (x)
#define SET_TIME_SS_DIGIT2(x) time[7] = (x)

#define VALIDATION_DATE_YEAR_MASK 0x80
#define VALIDATION_DATE_MONTH_MASK 0x40
#define VALIDATION_DATE_DAY_OF_MONTH_MASK 0x20
#define VALIDATION_DATE_DAY_OF_WEEK_MASK 0x10
#define VALIDATION_TIME_HOUR_MASK 0x08
#define VALIDATION_TIME_MINUTE_MASK 0x04
#define VALIDATION_END_OF_DATA 0x02
#define VALIDATION_ALL (0xff & ~0x01)

typedef void (*VoidBitHandler)(unsigned char bitNumber, unsigned char a, unsigned char b);
typedef VoidBitHandler (*BitHandler)(unsigned char bitNumber, unsigned char a, unsigned char b);

static BitHandler ignoreBits(unsigned char bitNumber, unsigned char a, unsigned char b);
static void showMinuteMarkerOnDebugScreen(void);
static void transitionToDebugScreen(void);
static void showDebugOutput(ScreenCallback showDebug);
static BitHandler setDutBits(unsigned char bitNumber, unsigned char a, unsigned char b);
static void showAllDecodingBufferBitsOnSecondLine(void);
static void showDecodingBufferBits(unsigned int mask, unsigned int numberOfBits);
static BitHandler setYearBits(unsigned char bitNumber, unsigned char a, unsigned char b);
static void showDate(void);
static BitHandler setMonthBits(unsigned char bitNumber, unsigned char a, unsigned char b);
static BitHandler setDayOfMonthBits(unsigned char bitNumber, unsigned char a, unsigned char b);
static BitHandler setDayOfWeekBits(unsigned char bitNumber, unsigned char a, unsigned char b);
static BitHandler setHourBits(unsigned char bitNumber, unsigned char a, unsigned char b);
static void showTime(void);
static BitHandler setMinuteBits(unsigned char bitNumber, unsigned char a, unsigned char b);
static BitHandler setYearParityBit(unsigned char bitNumber, unsigned char a, unsigned char b);
static void showValidationBits(void);
static void showValidationBitsOnSecondLine(void);
static BitHandler setMonthAndDayOfMonthParityBit(unsigned char bitNumber, unsigned char a, unsigned char b);
static BitHandler setDayOfWeekParityBit(unsigned char bitNumber, unsigned char a, unsigned char b);
static BitHandler setTimeParityBit(unsigned char bitNumber, unsigned char a, unsigned char b);
static BitHandler setDaylightSavingsBit(unsigned char bitNumber, unsigned char a, unsigned char b);
static void setClockSynchronisationState(unsigned char isDaylightSavings);

static const char *const daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static const char *const unknownDayOfWeek = "Today";

static unsigned char resetState;
static BitHandler bitHandler;
static unsigned int decodingBuffer;
static char decodingBufferAsString[17];
static char date[11];
static const char *dayOfWeekAsString;
static char time[9];

static unsigned char yearParity;
static unsigned char monthAndDayOfMonthParity;
static unsigned char dayOfWeekParity;
static unsigned char timeParity;

static unsigned char possiblyValidDateTimeParts;
static unsigned char invalidDateTimeParts;

static unsigned int decodedYear;
static unsigned char decodedMonth;
static unsigned char decodedDayOfMonth;
static unsigned char decodedDayOfWeek;
static unsigned char decodedHour;
static unsigned char decodedMinute;

void msfDecoderInitialise(void)
{
	possiblyValidDateTimeParts = 0;
	invalidDateTimeParts = ~0;
	resetState = 1;
	bitHandler = (BitHandler) ignoreBits;
}

static BitHandler ignoreBits(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	return (BitHandler) ignoreBits;
}

void msfDecoderMinuteMarker(void)
{
	possiblyValidDateTimeParts = 0;
	invalidDateTimeParts = ~0;
	resetState = 1;
	bitHandler = (BitHandler) setDutBits;
	showDebugOutput(showMinuteMarkerOnDebugScreen);
}

static void showDebugOutput(ScreenCallback showDebug)
{
	if (menuIsVisible || !config.receiver.flags.showDecodingDebug)
		return;

	showDebug();
}

static void showMinuteMarkerOnDebugScreen(void)
{
	screenPutStringsOnRightBothLines(
		"Minute sync'd...",
		"",
		transitionToDebugScreen);
}

static void transitionToDebugScreen(void)
{
	screenTransitionToRight(NO_CALLBACK);
}

int msfDecoderPushBits(unsigned char a, unsigned char b)
{
	static unsigned char bitNumber;
	if (resetState)
	{
		bitNumber = 0;
		resetState = 0;
	}

	bitHandler = (BitHandler) bitHandler(++bitNumber, a, b);
	return IS_VALID(VALIDATION_ALL);
}

static BitHandler setDutBits(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (bitNumber == 1)
		decodingBuffer = 0;

	if (b)
		decodingBuffer |= 1 << (16 - bitNumber);

	showDebugOutput(showAllDecodingBufferBitsOnSecondLine);
	return (BitHandler) (bitNumber < 16 ? setDutBits : setYearBits);
}

static void showAllDecodingBufferBitsOnSecondLine(void)
{
	showDecodingBufferBits(0x8000, 16);
}

static void showDecodingBufferBits(unsigned int mask, unsigned int numberOfBits)
{
	unsigned int i;

	for (i = 0; i < 16; i++, mask >>= 1)
	{
		if (i < numberOfBits)
			decodingBufferAsString[i] = (decodingBuffer & mask) ? '1' : '0';
		else
			decodingBufferAsString[i] = '\0';
	}

	screenPutStringOnRightLine(SCREEN_SECOND_LINE, decodingBufferAsString, NO_CALLBACK);
}

static BitHandler setYearBits(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (bitNumber == 17)
	{
		memcpy(date, "2000-00-00", 11);
		decodingBuffer = 0;
		dayOfWeekAsString = unknownDayOfWeek;
		yearParity = 0;
	}

	if (a)
	{
		decodingBuffer |= 1 << (24 - bitNumber);
		yearParity++;
	}

	if (bitNumber == 24)
	{
		unsigned char highDigit = BCD_PAIR_HIGH_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		unsigned char lowDigit = BCD_PAIR_LOW_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		if (IS_VALID_BCD_DIGIT_CHAR(highDigit) && IS_VALID_BCD_DIGIT_CHAR(lowDigit))
		{
			unsigned int year = 2000 + BCD_PAIR_TO_BINARY(decodingBuffer);
			if (year >= 2015)
			{
				possiblyValidDateTimeParts |= VALIDATION_DATE_YEAR_MASK;
				decodedYear = year;
			}
		}

		SET_DATE_YY_DIGIT1(highDigit);
		SET_DATE_YY_DIGIT2(lowDigit);
		showDebugOutput(showDate);

		return (BitHandler) setMonthBits;
	}

	return (BitHandler) setYearBits;
}

static void showDate(void)
{
	screenPutStringsOnBothLines(date, dayOfWeekAsString, NO_CALLBACK);
}

static BitHandler setMonthBits(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (bitNumber == 25)
	{
		decodingBuffer = 0;
		monthAndDayOfMonthParity = 0;
	}

	if (a)
	{
		decodingBuffer |= 1 << (29 - bitNumber);
		monthAndDayOfMonthParity++;
	}

	if (bitNumber == 29)
	{
		unsigned char highDigit = BCD_PAIR_HIGH_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		unsigned char lowDigit = BCD_PAIR_LOW_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		if (IS_VALID_BCD_DIGIT_CHAR(highDigit) && IS_VALID_BCD_DIGIT_CHAR(lowDigit))
		{
			unsigned char month = BCD_PAIR_TO_BINARY(decodingBuffer);
			if (month >= 1 && month <= 12)
			{
				possiblyValidDateTimeParts |= VALIDATION_DATE_MONTH_MASK;
				decodedMonth = month;
			}
		}

		SET_DATE_MM_DIGIT1(highDigit);
		SET_DATE_MM_DIGIT2(lowDigit);
		showDebugOutput(showDate);
		return (BitHandler) setDayOfMonthBits;
	}

	return (BitHandler) setMonthBits;
}

static BitHandler setDayOfMonthBits(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (bitNumber == 30)
		decodingBuffer = 0;

	if (a)
	{
		decodingBuffer |= 1 << (35 - bitNumber);
		monthAndDayOfMonthParity++;
	}

	if (bitNumber == 35)
	{
		unsigned char highDigit = BCD_PAIR_HIGH_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		unsigned char lowDigit = BCD_PAIR_LOW_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		if (IS_VALID_BCD_DIGIT_CHAR(highDigit) && IS_VALID_BCD_DIGIT_CHAR(lowDigit))
		{
			unsigned char dayOfMonth = BCD_PAIR_TO_BINARY(decodingBuffer);
			if (dayOfMonth >= 1 && dayOfMonth <= 31)
			{
				possiblyValidDateTimeParts |= VALIDATION_DATE_DAY_OF_MONTH_MASK;
				decodedDayOfMonth = dayOfMonth;
			}
		}

		SET_DATE_DD_DIGIT1(highDigit);
		SET_DATE_DD_DIGIT2(lowDigit);
		showDebugOutput(showDate);
		return (BitHandler) setDayOfWeekBits;
	}

	return (BitHandler) setDayOfMonthBits;
}

static BitHandler setDayOfWeekBits(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (bitNumber == 36)
	{
		decodingBuffer = 0;
		dayOfWeekParity = 0;
	}

	if (a)
	{
		decodingBuffer |= 1 << (38 - bitNumber);
		dayOfWeekParity++;
	}

	if (bitNumber == 38)
	{
		dayOfWeekAsString = (decodingBuffer <= 6) ? daysOfWeek[decodingBuffer] : unknownDayOfWeek;
		if (dayOfWeekAsString != unknownDayOfWeek)
		{
			possiblyValidDateTimeParts |= VALIDATION_DATE_DAY_OF_WEEK_MASK;
			decodedDayOfWeek = decodingBuffer;
		}

		showDebugOutput(showDate);
		return (BitHandler) setHourBits;
	}

	return (BitHandler) setDayOfWeekBits;
}

static BitHandler setHourBits(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (bitNumber == 39)
	{
		memcpy(time, "00:00:00", 9);
		decodingBuffer = 0;
		timeParity = 0;
	}

	if (a)
	{
		decodingBuffer |= 1 << (44 - bitNumber);
		timeParity++;
	}

	if (bitNumber == 44)
	{
		unsigned char highDigit = BCD_PAIR_HIGH_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		unsigned char lowDigit = BCD_PAIR_LOW_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		if (IS_VALID_BCD_DIGIT_CHAR(highDigit) && IS_VALID_BCD_DIGIT_CHAR(lowDigit))
		{
			unsigned char hour = BCD_PAIR_TO_BINARY(decodingBuffer);
			if (hour >= 0 && hour <= 23)
			{
				possiblyValidDateTimeParts |= VALIDATION_TIME_HOUR_MASK;
				decodedHour = hour;
			}
		}

		SET_TIME_HH_DIGIT1(highDigit);
		SET_TIME_HH_DIGIT2(lowDigit);
		showDebugOutput(showTime);
		return (BitHandler) setMinuteBits;
	}

	return (BitHandler) setHourBits;
}

static void showTime(void)
{
	screenPutStringOnRightLine(SCREEN_SECOND_LINE, time, NO_CALLBACK);
}

static BitHandler setMinuteBits(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (bitNumber == 45)
		decodingBuffer = 0;

	if (a)
	{
		decodingBuffer |= 1 << (51 - bitNumber);
		timeParity++;
	}

	if (bitNumber == 51)
	{
		unsigned char highDigit = BCD_PAIR_HIGH_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		unsigned char lowDigit = BCD_PAIR_LOW_DIGIT_CHAR_OR_QUESTION(decodingBuffer);
		if (IS_VALID_BCD_DIGIT_CHAR(highDigit) && IS_VALID_BCD_DIGIT_CHAR(lowDigit))
		{
			unsigned char minute = BCD_PAIR_TO_BINARY(decodingBuffer);
			if (minute >= 0 && minute <= 59)
			{
				possiblyValidDateTimeParts |= VALIDATION_TIME_MINUTE_MASK;
				decodedMinute = minute;
			}
		}

		SET_TIME_MM_DIGIT1(highDigit);
		SET_TIME_MM_DIGIT2(lowDigit);
		showDebugOutput(showTime);
		return (BitHandler) setYearParityBit;
	}

	return (BitHandler) setMinuteBits;
}

static BitHandler setYearParityBit(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (bitNumber == 54)
	{
		if (b)
			yearParity++;

		if (IS_PARITY_ODD(yearParity) && IS_POSSIBLY_VALID(VALIDATION_DATE_YEAR_MASK))
			invalidDateTimeParts &= ~VALIDATION_DATE_YEAR_MASK;

		decodingBuffer = (b ? 0x8000 : 0x0000) | invalidDateTimeParts;
		showDebugOutput(showValidationBits);
		return (BitHandler) setMonthAndDayOfMonthParityBit;
	}

	decodingBuffer = 0;
	showDebugOutput(showValidationBits);

	return (BitHandler) setYearParityBit;
}

static void showValidationBits(void)
{
	screenPutStringOnRightLine(SCREEN_FIRST_LINE, "YMdT    YMDdHm  ", showValidationBitsOnSecondLine);
}

static void showValidationBitsOnSecondLine(void)
{
	showDecodingBufferBits(0x8000, 14);
}

static BitHandler setMonthAndDayOfMonthParityBit(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (b)
		monthAndDayOfMonthParity++;

	if (IS_PARITY_ODD(monthAndDayOfMonthParity) && IS_POSSIBLY_VALID(VALIDATION_DATE_MONTH_MASK | VALIDATION_DATE_DAY_OF_MONTH_MASK))
		invalidDateTimeParts &= ~(VALIDATION_DATE_MONTH_MASK | VALIDATION_DATE_DAY_OF_MONTH_MASK);

	decodingBuffer = (decodingBuffer & 0xff00) | (b ? 0x4000 : 0x0000) | invalidDateTimeParts;
	showDebugOutput(showValidationBits);

	return (BitHandler) setDayOfWeekParityBit;
}

static BitHandler setDayOfWeekParityBit(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (b)
		dayOfWeekParity++;

	if (IS_PARITY_ODD(dayOfWeekParity) && IS_POSSIBLY_VALID(VALIDATION_DATE_DAY_OF_WEEK_MASK))
		invalidDateTimeParts &= ~VALIDATION_DATE_DAY_OF_WEEK_MASK;

	decodingBuffer = (decodingBuffer & 0xff00) | (b ? 0x2000 : 0x0000) | invalidDateTimeParts;
	showDebugOutput(showValidationBits);

	return (BitHandler) setTimeParityBit;
}

static BitHandler setTimeParityBit(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	if (b)
		timeParity++;

	if (IS_PARITY_ODD(timeParity) && IS_POSSIBLY_VALID(VALIDATION_TIME_HOUR_MASK | VALIDATION_TIME_MINUTE_MASK))
		invalidDateTimeParts &= ~(VALIDATION_TIME_HOUR_MASK | VALIDATION_TIME_MINUTE_MASK);

	decodingBuffer = (decodingBuffer & 0xff00) | (b ? 0x1000 : 0x0000) | invalidDateTimeParts;
	showDebugOutput(showValidationBits);

	return (BitHandler) setDaylightSavingsBit;
}

static BitHandler setDaylightSavingsBit(unsigned char bitNumber, unsigned char a, unsigned char b)
{
	setClockSynchronisationState(b);
	invalidDateTimeParts &= ~VALIDATION_END_OF_DATA;
	return (BitHandler) ignoreBits;
}

static void setClockSynchronisationState(unsigned char isDaylightSavings)
{
	if (IS_VALID(VALIDATION_DATE_YEAR_MASK))
		clockYearAtNextSynchronisationIs(decodedYear);

	if (IS_VALID(VALIDATION_DATE_MONTH_MASK))
		clockMonthAtNextSynchronisationIs(decodedMonth - 1);

	if (IS_VALID(VALIDATION_DATE_DAY_OF_MONTH_MASK))
		clockDayOfMonthAtNextSynchronisationIs(decodedDayOfMonth - 1);

	if (IS_VALID(VALIDATION_DATE_DAY_OF_WEEK_MASK))
		clockDayOfWeekAtNextSynchronisationIs(decodedDayOfWeek);

	if (IS_VALID(VALIDATION_TIME_HOUR_MASK))
		clockHourAtNextSynchronisationIs(decodedHour);

	if (IS_VALID(VALIDATION_TIME_MINUTE_MASK))
		clockMinuteAtNextSynchronisationIs(decodedMinute);

	clockSecondsInMinuteAtNextSynchronisationIs(60);
	clockSecondAtNextSynchronisationIs(1);
	clockDaylightSavingsAtNextSynchronisationIs(isDaylightSavings);
}
