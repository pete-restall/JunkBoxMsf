#ifndef __JUNKBOXMSF_MCU_H
#define __JUNKBOXMSF_MCU_H

#define MCU_CRYSTAL_FREQUENCY_HZ 7372800ul
#define MCU_PLL_MULTIPLIER 16
#define MCU_CLOCK_FREQUENCY_HZ (MCU_CRYSTAL_FREQUENCY_HZ * MCU_PLL_MULTIPLIER)
#define MCU_INSTRUCTION_FREQUENCY_HZ (MCU_CLOCK_FREQUENCY_HZ / 4)
#define MCU_INSTRUCTION_TIME (1.0 / MCU_INSTRUCTION_FREQUENCY_HZ)

#define MCU_LOW_POWER_INSTRUCTION_FREQUENCY_HZ (7370000ul / 4)

#define nop() Nop()
#define nop2() { nop(); nop(); }

extern void mcuWaitMilliseconds(int milliseconds);
extern void mcuWaitMicroseconds(int microseconds);

#endif
