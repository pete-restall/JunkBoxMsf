#ifndef __JUNKBOXMSF_SCREEN_H
#define __JUNKBOXMSF_SCREEN_H
#include "JunkBoxMsf.h"

#define SCREEN_FIRST_LINE 0
#define SCREEN_SECOND_LINE 1

typedef UpdateStatus (*ScreenState)(void);
typedef void (*ScreenCallback)(void);

extern void screenInitialise(void);
extern ScreenState screenUpdate;
extern void screenEnable(void);
extern int screenTransitionToLeft(ScreenCallback callback);
extern int screenTransitionToRight(ScreenCallback callback);
extern int screenTransitionToOther(ScreenCallback callback);
extern int screenPutStringsOnLeftBothLines(const char *firstLine, const char *secondLine, ScreenCallback callback);
extern int screenPutStringsOnRightBothLines(const char *firstLine, const char *secondLine, ScreenCallback callback);
extern int screenPutStringsOnBothLines(const char *firstLine, const char *secondLine, ScreenCallback callback);
extern int screenPutStringOnLeftLine(unsigned int line, const char *str, ScreenCallback callback);
extern int screenPutStringOnRightLine(unsigned int line, const char *str, ScreenCallback callback);
extern int screenPutStringOnLine(unsigned int line, const char *str, ScreenCallback callback);
extern int screenPutStringsOnOtherBothLines(const char *firstLine, const char *secondLine, ScreenCallback callback);
extern int screenPutStringOnOtherLine(unsigned int line, const char *str, ScreenCallback callback);

#endif
