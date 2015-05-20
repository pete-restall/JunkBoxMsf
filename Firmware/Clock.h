#ifndef __JUNKBOXMSF_CLOCK_H
#define	__JUNKBOXMSF_CLOCK_H
#include "JunkBoxMsf.h"

#define RTC_CRYSTAL_FREQUENCY_HZ 32768
#define RTC_CRYSTAL_TOLERANCE_PPM 30

#define MILLISECONDS_TO_TICKS(x) ((unsigned int) (((x) * 0.001 * RTC_CRYSTAL_FREQUENCY_HZ) + 0.5))
#define NOW_IN_TICKS TMR1
#define TICKS_ELAPSED_SINCE(x) ((NOW_IN_TICKS - (x)) & ((RTC_CRYSTAL_FREQUENCY_HZ / 2) - 1))

#define SECONDS_TO_UPTIME(x) ((x) << 1)
#define MINUTES_TO_UPTIME(x) SECONDS_TO_UPTIME((x) * 60ul)
#define UPTIME_NOW uptime[currentUptimeIndex]
#define UPTIME_INTERVAL_SINCE(x) (UPTIME_NOW - (x))
#define UPTIME_SECONDS_SINCE(x) (UPTIME_INTERVAL_SINCE(x) >> 1)

typedef enum
{
	Month_January,
	Month_February,
	Month_March,
	Month_April,
	Month_May,
	Month_June,
	Month_July,
	Month_August,
	Month_September,
	Month_October,
	Month_November,
	Month_December
} Month;

typedef enum
{
	Day_Sunday,
	Day_Monday,
	Day_Tuesday,
	Day_Wednesday,
	Day_Thursday,
	Day_Friday,
	Day_Saturday
} Day;

typedef struct
{
	struct
	{
		unsigned int year;
		unsigned char month;
		unsigned char dayOfMonth;
		unsigned char dayOfWeek;
		union
		{
			struct
			{
				unsigned int daysInMonth : 6;
				unsigned int isDaylightSavings : 1;
				unsigned int isLeapYear : 1;
			} flags;

			unsigned char flagsAsPrimitive;
		};
	} date;

	struct
	{
		unsigned char hours;
		unsigned char minutes;
		unsigned char seconds;
		unsigned char secondsInMinute;
	} time;
} Clock;

typedef enum
{
	ClockDriftIndicator_UnderTenMilliseconds,
	ClockDriftIndicator_OverTenMilliseconds,
	ClockDriftIndicator_OverFiftyMilliseconds,
	ClockDriftIndicator_OverOneHundredMilliseconds,
	ClockDriftIndicator_OverOneSecond
} ClockDriftIndicator;

typedef UpdateStatus (*ClockState)(void);

extern volatile unsigned char currentUptimeIndex;
extern volatile unsigned long uptime[2];

extern void clockInitialise(void);
extern void clockEnable(void);
extern ClockState clockUpdate;
extern void clockYearAtNextSynchronisationIs(unsigned int year);
extern void clockMonthAtNextSynchronisationIs(unsigned char month);
extern void clockDayOfMonthAtNextSynchronisationIs(unsigned char day);
extern void clockDayOfWeekAtNextSynchronisationIs(unsigned char day);
extern void clockDaylightSavingsAtNextSynchronisationIs(unsigned char isDaylightSavings);
extern void clockHourAtNextSynchronisationIs(unsigned char hour);
extern void clockMinuteAtNextSynchronisationIs(unsigned char minute);
extern void clockSecondAtNextSynchronisationIs(unsigned char second);
extern void clockSecondsInMinuteAtNextSynchronisationIs(unsigned char secondsInMinute);
extern void clockSynchronise(void);
extern ClockDriftIndicator clockDriftIndicator(void);
extern void clockShow(void);
extern void clockHide(void);

#endif
