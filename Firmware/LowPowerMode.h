#ifndef __JUNKBOXMSF_LOWPOWERMODE_H
#define	__JUNKBOXMSF_LOWPOWERMODE_H

extern void lowPowerModeInitialise(void);
extern void lowPowerModeEnable(void);
extern void lowPowerModeDisable(void);
extern int lowPowerModeIsEnabled(void);
extern void lowPowerModeGoToSleep(void);

#endif
