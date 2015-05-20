#include <xc.h>

#include "Mcu.h"
#define FOSC MCU_CRYSTAL_FREQUENCY_HZ
#define FCY MCU_INSTRUCTION_FREQUENCY_HZ
#include <libpic30.h>

void mcuWaitMilliseconds(int milliseconds)
{
	int i;
	for (i = 0; i < milliseconds; i++)
		__delay_ms(1);
}

void mcuWaitMicroseconds(int microseconds)
{
	int i;
	for (i = 0; i < microseconds; i++)
		__delay_us(1);
}
