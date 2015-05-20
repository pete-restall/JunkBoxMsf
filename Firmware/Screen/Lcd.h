#ifndef __JUNKBOXMSF_SCREEN_LCD_H
#define __JUNKBOXMSF_SCREEN_LCD_H
#include "../JunkBoxMsf.h"

typedef UpdateStatus (*LcdState)(void);
typedef void (*LcdCallback)(void);

extern void lcdInitialise(void);
extern void lcdEnable(void);
extern LcdState lcdUpdate;
extern int lcdSetCursorPosition(unsigned char x, unsigned char y, LcdCallback callback);
extern int lcdPutStringPadded(const char *str, unsigned int length, LcdCallback callback);
extern int lcdShiftRight(LcdCallback callback);
extern int lcdShiftLeft(LcdCallback callback);
extern void lcdBacklightOn(void);
extern void lcdBacklightOff(void);
extern int lcdSetContrast(unsigned int step);
extern void lcdLowPowerModeEnable(void);
extern void lcdLowPowerModeDisable(void);
extern void lcdOn(void);
extern void lcdOff(void);

#endif
