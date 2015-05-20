#ifndef __JUNKBOXMSF_MENU_H
#define	__JUNKBOXMSF_MENU_H
#include "JunkBoxMsf.h"

typedef UpdateStatus (*MenuState)(void);

extern unsigned char menuIsVisible;

extern void menuInitialise(void);
extern void menuEnable(void);
extern MenuState menuUpdate;

#endif
