#include <xc.h>

#include "Button.h"
#include "Clock.h"
#include "Config.h"
#include "LowPowerMode.h"
#include "Mcu.h"
#include "Menu.h"
#include "Receiver.h"
#include "Screen.h"

static void initialise(void);
static void run(void);
static void showWelcome(void);
static void delayAfterWelcomeMessageShown(void);
static unsigned char eventUpdates(void);
static void runEventLoop(void);

int main(void)
{
	SET_CPU_IPL(7);
	IEC0 = 0;
	IEC1 = 0;
	IEC2 = 0;

	TRISB = 0xff;
	TRISC = 0xff;
	TRISD = 0xff;
	TRISE = 0xff;
	TRISF = 0xff;

	initialise();
	run();
	return 0;
}

static void initialise(void)
{
	lowPowerModeInitialise();
	configInitialise();
	screenInitialise();
	clockInitialise();
	receiverInitialise();
	buttonInitialise();
	menuInitialise();
}

static void run(void)
{
	SET_CPU_IPL(0);
	clockEnable();
	screenEnable();
	menuEnable();
	showWelcome();
	receiverEnable();
	clockShow();
	runEventLoop();
}

static void showWelcome(void)
{
	screenPutStringsOnBothLines(
		"  Junk Box MSF  ",
		"pete@restall.net",
		NO_CALLBACK);

	delayAfterWelcomeMessageShown();
}

static void delayAfterWelcomeMessageShown(void)
{
	unsigned int milliseconds = 0, lastInterval = 0;
	lastInterval = NOW_IN_TICKS;
	while (milliseconds < 5000)
	{
		if (TICKS_ELAPSED_SINCE(lastInterval) >= MILLISECONDS_TO_TICKS(100))
		{
			milliseconds += 100;
			lastInterval = NOW_IN_TICKS;
		}

		eventUpdates();
	}
}

static unsigned char eventUpdates(void)
{
	return
		receiverUpdate() +
		clockUpdate() +
		screenUpdate() +
		buttonUpdate() +
		menuUpdate();
}

static void runEventLoop(void)
{
	unsigned char workRemaining, noWorkRemainingCount = 0;
	while (1)
	{
		workRemaining = eventUpdates();
		if (!workRemaining)
		{
			if (++noWorkRemainingCount >= 2)
			{
				noWorkRemainingCount = 0;
				lowPowerModeGoToSleep();
			}
		}
		else
			noWorkRemainingCount = 0;
	}
}
