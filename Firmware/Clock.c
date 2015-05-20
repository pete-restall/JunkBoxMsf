#include <xc.h>
#include <string.h>

#include "Clock.h"
#include "JunkBoxMsf.h"
#include "Mcu.h"
#include "NumberFormatting.h"
#include "Receiver.h"
#include "Screen.h"
#include "Screen/Lcd.h"

#define T1CON_TCS_EXTERNAL_CLOCK 1

#define IS_LEAP_YEAR(year) (((year) % 400 == 0) || ((year) % 100 != 0 && (year) % 4 == 0))

#define TIME_HH_POSITION 0
#define TIME_MM_POSITION 3
#define TIME_SS_POSITION 6

#define DATE_DD_POSITION 8
#define DATE_MM_POSITION 5
#define DATE_YYYY_POSITION 0

#define SET_DAY_OF_WEEK(a, b, c) { dateAsString[11] = (a); dateAsString[12] = (b); dateAsString[13] = (c); }
#define SET_TIMEZONE(a, b, c) { timeAsString[9] = (a); timeAsString[10] = (b); timeAsString[11] = (c); }
#define SET_RECEIVER_STATUS(x) dateAsString[15] = (x)
#define SET_RECEIVER_CHANNEL(x) timeAsString[15] = (x)
#define SET_DRIFT_INDICATOR(x) timeAsString[14] = driftIndicatorCharacters[(x)]

typedef union
{
	unsigned int asWord;

	struct
	{
		unsigned char current;
		unsigned char next;
	};
} ClockIndex;

typedef union
{
	unsigned char asChar;

	struct
	{
		unsigned int unused : 3;
		unsigned int synchronised : 1;
		unsigned int synchronise : 1; /* Use the same bit position (#4) in GoertzelDetectorIsr.s, too */
		unsigned int tock : 1;
		unsigned int tick : 1;
		unsigned int minuteMarker : 1;
	};
} ClockFlags;

static void rtcOscillatorEnable(void);
static UpdateStatus clockStateInTick(void);
static UpdateStatus clockStateSynchronised(void);
static UpdateStatus clockStatePrepareForNextMinute(void);
static UpdateStatus clockStateUpdateOnScreen(void);
static void updateDateOnScreen(void);
static int updateDateInBuffer(void);
static UpdateStatus clockStateInTock(void);
static UpdateStatus clockStateSynchronisedAndWaitingForTock(void);

volatile unsigned char currentUptimeIndex;
volatile unsigned long uptime[2];
volatile ClockFlags clockFlags;

ClockState clockUpdate;
static ClockState clockStateNext;

static volatile Clock clocks[3];
static volatile ClockIndex clockIndex;
static volatile unsigned int clockDriftTickCounter;

static unsigned char clockIsVisible;
static char dateAsString[17];
static char timeAsString[17];
static const unsigned char daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const char *const daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static const char *const driftIndicatorCharacters = " \xdf\xe0\xe1!";

static struct
{
	unsigned char dayOfWeek;
	unsigned char dayOfMonth;
	unsigned char month;
	unsigned int year;
	unsigned char flags;
	unsigned char receiverStatusFlags;
} previouslyDisplayedDateLine;

void clockInitialise(void)
{
	PMD1bits.T1MD = 0;
	nop2();

	T1CONbits.TON = 0;
	T1CONbits.TCS = T1CON_TCS_EXTERNAL_CLOCK;
	rtcOscillatorEnable();
	PR1 = (RTC_CRYSTAL_FREQUENCY_HZ / 2) - 1;
	IPC0bits.T1IP = IPC2bits.ADIP - 1;

	LATFbits.LATF2 = 0;
	TRISFbits.TRISF2 = 0;

	uptime[0] = 0;
	uptime[1] = 0;
	currentUptimeIndex = 0;

	clockUpdate = clockStateInTick;
	clockStateNext = clockUpdate;
	clockIsVisible = 0;

	memcpy(dateAsString, "0000-00-00 Sun  ", 17);
	memcpy(timeAsString, "00:00:00 GMT    ", 17);
	memset(&previouslyDisplayedDateLine, 0xff, sizeof(previouslyDisplayedDateLine));
}

static void rtcOscillatorEnable(void)
{
	__builtin_write_OSCCONL(OSCCONL | _OSCCON_LPOSCEN_MASK);
}

static UpdateStatus clockStateInTick(void)
{
	if (clockFlags.synchronised)
	{
		clockUpdate = clockStateSynchronised;
		return UpdateStatus_MoreWorkQueued;
	}

	if (clockFlags.minuteMarker)
	{
		clockUpdate = clockStatePrepareForNextMinute;
		return UpdateStatus_MoreWorkQueued;
	}

	if (clockFlags.tock)
	{
		clockUpdate = clockStateUpdateOnScreen;
		clockStateNext = clockStateInTock;
		return UpdateStatus_MoreWorkQueued;
	}

	return UpdateStatus_Idle;
}

static UpdateStatus clockStateSynchronised(void)
{
	clockStatePrepareForNextMinute();
	clockStateNext = clockStateSynchronisedAndWaitingForTock;
	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus clockStatePrepareForNextMinute(void)
{
	ClockIndex index = {clockIndex.asWord};
	volatile Clock *nextMinute = &clocks[index.next];
	memcpy((void *) nextMinute, (void *) &clocks[index.current], sizeof(Clock));

	nextMinute->time.seconds = 0;
	nextMinute->time.secondsInMinute = 60;
	if (++(nextMinute->time.minutes) == 60)
	{
		nextMinute->time.minutes = 0;
		if (++(nextMinute->time.hours) == 24)
		{
			nextMinute->time.hours = 0;
			if (++(nextMinute->date.dayOfMonth) == nextMinute->date.flags.daysInMonth)
			{
				nextMinute->date.dayOfMonth = 0;
				if (++(nextMinute->date.month) == 12)
				{
					nextMinute->date.month = 0;
					nextMinute->date.year++;
					nextMinute->date.flags.isLeapYear = IS_LEAP_YEAR(nextMinute->date.year);
				}

				nextMinute->date.flags.daysInMonth = daysInMonth[nextMinute->date.month];
				if (nextMinute->date.month == Month_February && nextMinute->date.flags.isLeapYear)
					nextMinute->date.flags.daysInMonth = 29;
			}

			if (++(nextMinute->date.dayOfWeek) == 7)
				nextMinute->date.dayOfWeek = 0;
		}
	}

	clockUpdate = clockStateUpdateOnScreen;
	clockStateNext = clockStateInTock;
	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus clockStateUpdateOnScreen(void)
{
	volatile Clock *clock = &clocks[clockIndex.current];
	ClockDriftIndicator drift = clockDriftIndicator();

	numberFormattingAsTwoDigits(clock->time.hours, timeAsString + TIME_HH_POSITION);
	numberFormattingAsTwoDigits(clock->time.minutes, timeAsString + TIME_MM_POSITION);
	numberFormattingAsTwoDigits(clock->time.seconds, timeAsString + TIME_SS_POSITION);

	if (receiverStatus.channel == 1)
		SET_RECEIVER_CHANNEL('_');
	else if (receiverStatus.channel == 2)
		SET_RECEIVER_CHANNEL(0x1c);
	else if (receiverStatus.channel == 3)
		SET_RECEIVER_CHANNEL(0x11);
	else
		SET_RECEIVER_CHANNEL(' ');

	SET_DRIFT_INDICATOR(drift);

	if (clockIsVisible)
		screenPutStringOnLeftLine(SCREEN_SECOND_LINE, timeAsString, updateDateOnScreen);

	clockUpdate = clockStateNext;

	return UpdateStatus_MoreWorkQueued;
}

static void updateDateOnScreen(void)
{
	if (updateDateInBuffer())
		screenPutStringOnLeftLine(SCREEN_FIRST_LINE, dateAsString, NO_CALLBACK);
}

static int updateDateInBuffer(void)
{
	volatile Clock *clock = &clocks[clockIndex.current];
	int isUpdateScreenRequired = 0;

	if (clock->date.dayOfWeek != previouslyDisplayedDateLine.dayOfWeek)
	{
		const char *const dayOfWeek = daysOfWeek[clock->date.dayOfWeek <= 6 ? clock->date.dayOfWeek : 6];
		SET_DAY_OF_WEEK(dayOfWeek[0], dayOfWeek[1], dayOfWeek[2]);
		previouslyDisplayedDateLine.dayOfWeek = clock->date.dayOfWeek;
		isUpdateScreenRequired = 1;
	}

	if (clock->date.dayOfMonth != previouslyDisplayedDateLine.dayOfMonth)
	{
		numberFormattingAsTwoDigits(clock->date.dayOfMonth + 1, dateAsString + DATE_DD_POSITION);
		previouslyDisplayedDateLine.dayOfMonth = clock->date.dayOfMonth;
		isUpdateScreenRequired = 1;
	}

	if (clock->date.month != previouslyDisplayedDateLine.month)
	{
		numberFormattingAsTwoDigits(clock->date.month + 1, dateAsString + DATE_MM_POSITION);
		previouslyDisplayedDateLine.month = clock->date.month;
		isUpdateScreenRequired = 1;
	}

	if (clock->date.year != previouslyDisplayedDateLine.year)
	{
		numberFormattingAsFourDigits(clock->date.year, dateAsString + DATE_YYYY_POSITION);
		previouslyDisplayedDateLine.year = clock->date.year;
		isUpdateScreenRequired = 1;
	}

	if (clock->date.flagsAsPrimitive != previouslyDisplayedDateLine.flags)
	{
		if (clock->date.flags.isDaylightSavings)
		{
			SET_TIMEZONE('B', 'S', 'T');
		}
		else
		{
			SET_TIMEZONE('G', 'M', 'T');
		}

		previouslyDisplayedDateLine.flags = clock->date.flagsAsPrimitive;
		isUpdateScreenRequired = 1;
	}

	if (receiverStatus.flagsAsPrimitive != previouslyDisplayedDateLine.receiverStatusFlags)
	{
		/* Assuming an ST7066 character set... */

		if (receiverStatus.flags.synchronised)
			SET_RECEIVER_STATUS(0xfe);
		else if (receiverStatus.flags.failedToSynchronise)
			SET_RECEIVER_STATUS(0xb7);
		else if (receiverStatus.flags.decoding)
			SET_RECEIVER_STATUS(0xf5);
		else if (receiverStatus.flags.waitingForSynchronisation)
			SET_RECEIVER_STATUS(0xd8);
		else if (receiverStatus.flags.on)
			SET_RECEIVER_STATUS(0xdd);
		else
			SET_RECEIVER_STATUS(' ');

		previouslyDisplayedDateLine.receiverStatusFlags = receiverStatus.flagsAsPrimitive;
		isUpdateScreenRequired = 1;
	}

	return isUpdateScreenRequired;
}

static UpdateStatus clockStateInTock(void)
{
	if (clockFlags.synchronised)
	{
		clockUpdate = clockStateSynchronised;
		return UpdateStatus_MoreWorkQueued;
	}

	if (clockFlags.tick)
	{
		clockUpdate = clockStateInTick;
		return UpdateStatus_MoreWorkQueued;
	}

	return UpdateStatus_Idle;
}

static UpdateStatus clockStateSynchronisedAndWaitingForTock(void)
{
	if (clockFlags.tock)
	{
		clockUpdate = clockStateUpdateOnScreen;
		clockStateNext = clockStateInTock;
		return UpdateStatus_MoreWorkQueued;
	}

	return UpdateStatus_Idle;
}

void clockEnable(void)
{
	clockIndex.current = 0;
	clockIndex.next = 1;
	clockFlags.asChar = 0;
	clockFlags.tick = 1;
	clockDriftTickCounter = 0;
	clockUpdate = clockStatePrepareForNextMinute;
	LATFbits.LATF2 = 0;

	memset((void *) clocks, 0, sizeof(clocks));
	clocks[clockIndex.current].time.secondsInMinute = 60;

	TMR1 = 0;
	IFS0bits.T1IF = 0;
	IEC0bits.T1IE = 1;
	T1CONbits.TON = 1;
}

void clockYearAtNextSynchronisationIs(unsigned int year)
{
	volatile Clock *nextMinute = &clocks[2];
	nextMinute->date.year = year;
	nextMinute->date.flags.isLeapYear = IS_LEAP_YEAR(year);
	if (nextMinute->date.flags.isLeapYear && nextMinute->date.month == Month_February)
		nextMinute->date.flags.daysInMonth = 29;
}

void clockMonthAtNextSynchronisationIs(unsigned char month)
{
	volatile Clock *nextMinute = &clocks[2];
	nextMinute->date.month = month;
	nextMinute->date.flags.daysInMonth = daysInMonth[month];
	if (month == Month_February && nextMinute->date.flags.isLeapYear)
		nextMinute->date.flags.daysInMonth = 29;
}

void clockDayOfMonthAtNextSynchronisationIs(unsigned char day)
{
	clocks[2].date.dayOfMonth = day;
}

void clockDayOfWeekAtNextSynchronisationIs(unsigned char day)
{
	clocks[2].date.dayOfWeek = day;
}

void clockDaylightSavingsAtNextSynchronisationIs(unsigned char isDaylightSavings)
{
	clocks[2].date.flags.isDaylightSavings = isDaylightSavings;
}

void clockHourAtNextSynchronisationIs(unsigned char hour)
{
	clocks[2].time.hours = hour;
}

void clockMinuteAtNextSynchronisationIs(unsigned char minute)
{
	clocks[2].time.minutes = minute;
}

void clockSecondAtNextSynchronisationIs(unsigned char second)
{
	clocks[2].time.seconds = second;
}

void clockSecondsInMinuteAtNextSynchronisationIs(unsigned char secondsInMinute)
{
	clocks[2].time.secondsInMinute = secondsInMinute;
}

void clockSynchronise(void)
{
	TMR1 = 0;
	clockFlags.synchronise = 1;
}

ClockDriftIndicator clockDriftIndicator(void)
{
	if (clockDriftTickCounter >= RTC_CRYSTAL_FREQUENCY_HZ)
		return ClockDriftIndicator_OverOneSecond;

	if (clockDriftTickCounter >= RTC_CRYSTAL_FREQUENCY_HZ / 10)
		return ClockDriftIndicator_OverOneHundredMilliseconds;

	if (clockDriftTickCounter >= RTC_CRYSTAL_FREQUENCY_HZ / 20)
		return ClockDriftIndicator_OverFiftyMilliseconds;

	if (clockDriftTickCounter >= RTC_CRYSTAL_FREQUENCY_HZ / 100)
		return ClockDriftIndicator_OverTenMilliseconds;

	return ClockDriftIndicator_UnderTenMilliseconds;
}

void clockShow(void)
{
	clockIsVisible = ~0;
}

void clockHide(void)
{
	clockIsVisible = 0;
}

void __attribute__((__interrupt__(__no_auto_psv__))) _T1Interrupt(void)
{
	static unsigned char clockDriftMillionTicksCounter = 0;

	/* This ISR can itself be interrupted by the ADC interrupt, which also modifies clockFlags - so any clockFlags
	   operations that would not preserve values between instructions need to be inline assembly; check the compiler's
	   assembly listing of this function to make sure it can be interrupted safely without losing any flags...!!! */

	__asm__ volatile ("btg LATF, #2");
	IFS0bits.T1IF = 0;
	uptime[currentUptimeIndex ^ 1] = uptime[currentUptimeIndex] + 1;
	currentUptimeIndex ^= 1;

	if (++clockDriftMillionTicksCounter == (1000000 / (RTC_CRYSTAL_FREQUENCY_HZ / 2)))
	{
		unsigned int newClockDriftTickCounter = clockDriftTickCounter + RTC_CRYSTAL_TOLERANCE_PPM;
		if (newClockDriftTickCounter < clockDriftTickCounter)
			clockDriftTickCounter = 0xffff;
		else
			clockDriftTickCounter = newClockDriftTickCounter;

		clockDriftMillionTicksCounter = 0;
	}

	if (clockFlags.synchronise)
	{
		clockIndex.current = 2;
		clockIndex.next = 0;
		clockFlags.asChar = 0;
		clockFlags.synchronised = 1;
		clockDriftMillionTicksCounter = 0;
		clockDriftTickCounter = 0;
		LATFbits.LATF2 = 0;
		return;
	}

	clockFlags.minuteMarker = 0;
	clockFlags.synchronised = 0;
	if (clockFlags.tock)
	{
		clockFlags.tick = 1;
		clockFlags.tock = 0;
		return;
	}

	clockFlags.tick = 0;
	clockFlags.tock = 1;
	if (++clocks[clockIndex.current].time.seconds < clocks[clockIndex.next].time.secondsInMinute)
		return;

	clockIndex.current = clockIndex.next;
	clockIndex.next ^= 1;
	clockFlags.minuteMarker = 1;
}
