#include <xc.h>

#include "LowPowerMode.h"
#include "Mcu.h"

#include "Screen/Lcd.h"

#if MCU_INSTRUCTION_FREQUENCY_HZ != 29491200ul
#error The values in this file need reviewing as they are calibrated to a different (primary) clock frequency !
#endif

#define PMD_ALL_PERIPHERALS_EXCEPT_RTC_DISABLED ( \
	_PMD1_ADCMD_MASK | _PMD1_I2CMD_MASK | _PMD1_PWMMD_MASK | _PMD1_QEIMD_MASK | \
	_PMD1_SPI1MD_MASK | _PMD1_T2MD_MASK | _PMD1_T3MD_MASK | _PMD1_U1MD_MASK)

void lowPowerModeInitialise(void)
{
	PMD1 = 0xffff;
	PMD2 = 0xffff;
}

void lowPowerModeEnable(void)
{
	__builtin_write_OSCCONH(((OSCCON & ~_OSCCON_NOSC_MASK) | _OSCCON_NOSC0_MASK) >> 8);
	__builtin_write_OSCCONL(OSCCONL | _OSCCON_OSWEN_MASK);
	lcdLowPowerModeEnable();
}

int lowPowerModeIsEnabled(void)
{
	return (OSCCON & (_OSCCON_COSC_MASK | _OSCCON_OSWEN_MASK)) == _OSCCON_COSC0_MASK;
}

void lowPowerModeDisable(void)
{
	__builtin_write_OSCCONH(((OSCCON & ~_OSCCON_NOSC_MASK) | _OSCCON_NOSC1_MASK | _OSCCON_NOSC0_MASK) >> 8);
	__builtin_write_OSCCONL(OSCCONL | _OSCCON_OSWEN_MASK);
	lcdLowPowerModeDisable();
}

void lowPowerModeGoToSleep(void)
{
	if (PMD1 == PMD_ALL_PERIPHERALS_EXCEPT_RTC_DISABLED)
	{
		Sleep();
	}
	else
	{
		Idle();
	}
}
