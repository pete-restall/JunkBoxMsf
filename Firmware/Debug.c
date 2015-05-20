#include "Debug.h"
#include "Clock.h"

#if DEBUG_RECEIVER_STUB_DATE_AND_TIME
void debugReceiverStubDateAndTime(void)
{
	clockYearAtNextSynchronisationIs(2015);
	clockMonthAtNextSynchronisationIs(Month_December);
	clockDayOfMonthAtNextSynchronisationIs(30);
	clockDayOfWeekAtNextSynchronisationIs(Day_Saturday);
	clockHourAtNextSynchronisationIs(23);
	clockMinuteAtNextSynchronisationIs(59);
	clockSecondAtNextSynchronisationIs(55);
	clockSecondsInMinuteAtNextSynchronisationIs(60);
	clockDaylightSavingsAtNextSynchronisationIs(0);
	clockSynchronise();
}
#endif
