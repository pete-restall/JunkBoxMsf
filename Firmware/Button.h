#ifndef __JUNKBOXMSF_BUTTON_H
#define	__JUNKBOXMSF_BUTTON_H
#include "JunkBoxMsf.h"

typedef UpdateStatus (*ButtonState)(void);
typedef void (*ButtonPressCallback)(void);

extern void buttonInitialise(void);
extern ButtonState buttonUpdate;
extern void buttonOnShortPress(ButtonPressCallback callback);
extern void buttonOnLongPress(ButtonPressCallback callback);

#endif
