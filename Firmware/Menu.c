#include <xc.h>
#include <string.h>

#include "Button.h"
#include "JunkBoxMsf.h"
#include "Menu.h"
#include "NumberFormatting.h"
#include "Screen.h"
#include "NumberFormatting.h"
#include "Config.h"
#include "Screen/Lcd.h"
#include "Clock.h"

#define MENU_NAVIGATION_LINE "[L] OK     [S] \xf6"
#define MENU_ADJUSTMENT_LINE "[L] OK   [S] Adj"

#define ENTER_MENU(menuName) \
	{ \
		menu = menuName; \
		numberOfMenuItems = sizeof(menuName) / sizeof(MenuItem); \
		currentMenuItem = 0; \
		menuUpdate = menuStateShowCurrentItem; \
	}

#define LCD_CONTRAST_TEXT "Contrast: 00"
#define LCD_CONTRAST_POSITION 10

#define LCD_BACKLIGHT_TEXT "B/L Seconds: 00"
#define LCD_BACKLIGHT_POSITION 13
#define LCD_BACKLIGHT_MAXIMUM_SECONDS 30

#define LCD_LOW_POWER_TEXT "Awake: 00m00s"
#define LCD_LOW_POWER_MM_POSITION 7
#define LCD_LOW_POWER_SS_POSITION 10

#define RECEIVER_DEBUG_SHOW_DECODING_TEXT "Show: Yes"
#define RECEIVER_DEBUG_SHOW_DECODING_POSITION 6

#define RECEIVER_DEBUG_CARRIER_ON_IO_TEXT "PGD Pin: Yes"
#define RECEIVER_DEBUG_CARRIER_ON_IO_POSITION 9

#define RECEIVER_DEBUG_CARRIER_ON_BACKLIGHT_TEXT "Backlight: Yes"
#define RECEIVER_DEBUG_CARRIER_ON_BACKLIGHT_POSITION 11

#define RECEIVER_RESYNCHRONISATION_INTERVAL_TEXT "Every: 00h00m"
#define RECEIVER_RESYNCHRONISATION_INTERVAL_HH_POSITION 7
#define RECEIVER_RESYNCHRONISATION_INTERVAL_MM_POSITION 10

typedef enum
{
	ButtonPress_None,
	ButtonPress_Short,
	ButtonPress_Long
} ButtonPress;

typedef struct
{
	const char *const text;
	MenuState onSelected;
} MenuItem;

typedef void(*AdjustmentCallback)(void);

unsigned char menuIsVisible;
MenuState menuUpdate;
static MenuState menuNextStateAfterAdjustment;
static ButtonPress buttonPress;
static const MenuItem *menu;
static unsigned char numberOfMenuItems;
static unsigned char currentMenuItem;
static AdjustmentCallback makeAdjustment;
static AdjustmentCallback makeAdjustments[4];
static unsigned int makeAdjustmentsIndex;
static char menuAdjustmentText[16];

static UpdateStatus menuStateIdle(void);
static UpdateStatus menuStateEnterMainMenu(void);
static UpdateStatus menuStateShowCurrentItem(void);
static void transitionToRightScreen(void);
static UpdateStatus menuStateAwaitingEntryOrNext(void);
static UpdateStatus menuStateEnterLcdSettings(void);
static UpdateStatus menuStateEnterLcdContrastSettings(void);
static UpdateStatus menuStateShowAdjustment(void);
static UpdateStatus menuStateAwaitingAdjustmentOrNext(void);
static void makeLcdContrastAdjustment(void);
static UpdateStatus menuStateEnterLcdBacklightSettings(void);
static void makeLcdBacklightAdjustment(void);
static UpdateStatus menuStateEnterLcdLowPowerSettings(void);
static void showLcdLowPowerTimeout(void);
static UpdateStatus menuStateShowMultipleAdjustments(void);
static UpdateStatus menuStateMakeNextAdjustment(void);
static void makeLcdLowPowerMinutesAdjustment(void);
static void makeLcdLowPowerSecondsAdjustment(void);
static UpdateStatus menuStateEnterReceiverSettings(void);
static UpdateStatus menuStateEnterReceiverDebugSettings(void);
static UpdateStatus menuStateEnterReceiverResynchronisationIntervalSettings(void);
static void showReceiverResynchronisationInterval(void);
static void makeReceiverResynchronisationIntervalHoursAdjustment(void);
static void makeReceiverResynchronisationIntervalMinutesAdjustment(void);
static UpdateStatus menuStateEnterReceiverDebugShowDecodingSettings(void);
static void makeReceiverDebugShowDecodingAdjustment(void);
static UpdateStatus menuStateEnterReceiverDebugCarrierOnIoSettings(void);
static void makeReceiverDebugCarrierOnIoAdjustment(void);
static UpdateStatus menuStateEnterReceiverDebugCarrierOnBacklightSettings(void);
static void makeReceiverDebugCarrierOnBacklightAdjustment(void);
static UpdateStatus menuStateEnterExit(void);
static UpdateStatus menuStateInExitEnterSave(void);
static UpdateStatus menuStateTransitionToLeftScreen(void);
static UpdateStatus menuStateWaitForTransitionToLeftScreen(void);
static UpdateStatus menuStateInExitEnterDoNotSave(void);
static void onButtonShortPress(void);
static void onButtonLongPress(void);

static const MenuItem mainMenu[] = {
	{"LCD", menuStateEnterLcdSettings},
	{"Receiver", menuStateEnterReceiverSettings},
	{"Exit", menuStateEnterExit}
};

static const MenuItem lcdMenu[] = {
	{"Contrast", menuStateEnterLcdContrastSettings},
	{"Backlight", menuStateEnterLcdBacklightSettings},
	{"Low Power", menuStateEnterLcdLowPowerSettings},
	{"Go Back", menuStateEnterMainMenu}
};

static const MenuItem receiverMenu[] = {
	{"Debug", menuStateEnterReceiverDebugSettings},
	{"Resync Interval", menuStateEnterReceiverResynchronisationIntervalSettings},
	{"Go Back", menuStateEnterMainMenu}
};

static const MenuItem receiverDebugMenu[] = {
	{"Show Decoding", menuStateEnterReceiverDebugShowDecodingSettings},
	{"Carrier on I/O", menuStateEnterReceiverDebugCarrierOnIoSettings},
	{"Carrier on B/L", menuStateEnterReceiverDebugCarrierOnBacklightSettings},
	{"Go Back", menuStateEnterReceiverSettings}
};

static const MenuItem exitMenu[] = {
	{"Save to EEPROM", menuStateInExitEnterSave},
	{"Don't Save", menuStateInExitEnterDoNotSave},
	{"Go Back", menuStateEnterMainMenu}
};

void menuInitialise(void)
{
	buttonPress = ButtonPress_None;
	menuUpdate = menuStateIdle;
}

static UpdateStatus menuStateIdle(void)
{
	menuIsVisible = 0;
	if (buttonPress == ButtonPress_Long)
	{
		menuIsVisible = ~0;
		buttonPress = ButtonPress_None;
		return menuStateEnterMainMenu();
	}

	buttonPress = ButtonPress_None;
	return UpdateStatus_Idle;
}

static UpdateStatus menuStateEnterMainMenu(void)
{
	ENTER_MENU(mainMenu);
	return menuStateShowCurrentItem();
}

static UpdateStatus menuStateShowCurrentItem(void)
{
	if (screenPutStringsOnRightBothLines(menu[currentMenuItem].text, MENU_NAVIGATION_LINE, transitionToRightScreen))
		menuUpdate = menuStateAwaitingEntryOrNext;

	return UpdateStatus_MoreWorkQueued;
}

static void transitionToRightScreen(void)
{
	screenTransitionToRight(NO_CALLBACK);
}

static UpdateStatus menuStateAwaitingEntryOrNext(void)
{
	if (buttonPress == ButtonPress_Long)
	{
		buttonPress = ButtonPress_None;
		menuUpdate = menu[currentMenuItem].onSelected;
		return UpdateStatus_MoreWorkQueued;
	}
	else if (buttonPress == ButtonPress_Short)
	{
		buttonPress = ButtonPress_None;
		if (++currentMenuItem >= numberOfMenuItems)
			currentMenuItem = 0;

		menuUpdate = menuStateShowCurrentItem;
		return UpdateStatus_MoreWorkQueued;
	}

	return UpdateStatus_Idle;
}

static UpdateStatus menuStateEnterLcdSettings(void)
{
	ENTER_MENU(lcdMenu);
	return menuStateShowCurrentItem();
}

static UpdateStatus menuStateEnterLcdContrastSettings(void)
{
	memcpy(menuAdjustmentText, LCD_CONTRAST_TEXT, sizeof(LCD_CONTRAST_TEXT));
	numberFormattingAsTwoDigits(config.lcd.contrast, menuAdjustmentText + LCD_CONTRAST_POSITION);
	menuUpdate = menuStateShowAdjustment;
	makeAdjustment = makeLcdContrastAdjustment;
	return menuStateShowAdjustment();
}

static UpdateStatus menuStateShowAdjustment(void)
{
	if (screenPutStringsOnRightBothLines(menuAdjustmentText, MENU_ADJUSTMENT_LINE, transitionToRightScreen))
	{
		menuNextStateAfterAdjustment = menuStateShowCurrentItem;
		menuUpdate = menuStateAwaitingAdjustmentOrNext;
	}

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus menuStateAwaitingAdjustmentOrNext(void)
{
	static unsigned char hasPutUpdatedText;

	if (buttonPress == ButtonPress_Long)
	{
		buttonPress = ButtonPress_None;
		menuUpdate = menuNextStateAfterAdjustment;
		return menuNextStateAfterAdjustment();
	}

	if (buttonPress == ButtonPress_Short)
	{
		buttonPress = ButtonPress_None;
		makeAdjustment();
		hasPutUpdatedText = 0;
	}

	if (!hasPutUpdatedText)
		hasPutUpdatedText = screenPutStringOnRightLine(SCREEN_FIRST_LINE, menuAdjustmentText, transitionToRightScreen);

	return UpdateStatus_Idle;
}

static void makeLcdContrastAdjustment(void)
{
	if (lcdSetContrast(config.lcd.contrast++) > 0)
		config.lcd.contrast = 0;

	lcdSetContrast(config.lcd.contrast);
	numberFormattingAsTwoDigits(config.lcd.contrast, menuAdjustmentText + LCD_CONTRAST_POSITION);
}

static UpdateStatus menuStateEnterLcdBacklightSettings(void)
{
	memcpy(menuAdjustmentText, LCD_BACKLIGHT_TEXT, sizeof(LCD_BACKLIGHT_TEXT));
	numberFormattingAsTwoDigits(
		config.lcd.backlightSecondsAfterButtonPress,
		menuAdjustmentText + LCD_BACKLIGHT_POSITION);

	menuUpdate = menuStateShowAdjustment;
	makeAdjustment = makeLcdBacklightAdjustment;
	return menuStateShowAdjustment();
}

static void makeLcdBacklightAdjustment(void)
{
	if (++config.lcd.backlightSecondsAfterButtonPress > LCD_BACKLIGHT_MAXIMUM_SECONDS)
		config.lcd.backlightSecondsAfterButtonPress = 0;

	numberFormattingAsTwoDigits(
		config.lcd.backlightSecondsAfterButtonPress,
		menuAdjustmentText + LCD_BACKLIGHT_POSITION);
}

static UpdateStatus menuStateEnterLcdLowPowerSettings(void)
{
	memcpy(menuAdjustmentText, LCD_LOW_POWER_TEXT, sizeof(LCD_LOW_POWER_TEXT));

	showLcdLowPowerTimeout();

	menuUpdate = menuStateShowMultipleAdjustments;
	makeAdjustments[0] = makeLcdLowPowerMinutesAdjustment;
	makeAdjustments[1] = makeLcdLowPowerSecondsAdjustment;
	makeAdjustments[2] = NO_CALLBACK;
	return menuStateShowMultipleAdjustments();
}

static void showLcdLowPowerTimeout(void)
{
	unsigned long minutes = config.lcd.lowPowerTimeoutAfterButtonPress / MINUTES_TO_UPTIME(1);
	unsigned long seconds = config.lcd.lowPowerTimeoutAfterButtonPress % MINUTES_TO_UPTIME(1) / SECONDS_TO_UPTIME(1);

	numberFormattingAsTwoDigits(minutes, menuAdjustmentText + LCD_LOW_POWER_MM_POSITION);
	numberFormattingAsTwoDigits(seconds, menuAdjustmentText + LCD_LOW_POWER_SS_POSITION);
}

static UpdateStatus menuStateShowMultipleAdjustments(void)
{
	if (screenPutStringsOnRightBothLines(menuAdjustmentText, MENU_ADJUSTMENT_LINE, transitionToRightScreen))
	{
		makeAdjustmentsIndex = 0;
		makeAdjustment = makeAdjustments[0];
		menuNextStateAfterAdjustment = menuStateMakeNextAdjustment;
		menuUpdate = menuStateAwaitingAdjustmentOrNext;
	}

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus menuStateMakeNextAdjustment(void)
{
	makeAdjustment = makeAdjustments[++makeAdjustmentsIndex];
	if (!makeAdjustment)
		menuUpdate = menuStateShowCurrentItem;
	else
		menuUpdate = menuStateAwaitingAdjustmentOrNext;

	return UpdateStatus_MoreWorkQueued;
}

static void makeLcdLowPowerMinutesAdjustment(void)
{
	unsigned long timeout = config.lcd.lowPowerTimeoutAfterButtonPress + MINUTES_TO_UPTIME(5);
	unsigned long minutes = timeout / MINUTES_TO_UPTIME(1);

	if (minutes > 60)
		timeout -= MINUTES_TO_UPTIME(minutes);

	config.lcd.lowPowerTimeoutAfterButtonPress = timeout;
	showLcdLowPowerTimeout();
}

static void makeLcdLowPowerSecondsAdjustment(void)
{
	unsigned long seconds = config.lcd.lowPowerTimeoutAfterButtonPress % MINUTES_TO_UPTIME(1) / SECONDS_TO_UPTIME(1);

	if (seconds + 5 <= 59)
		config.lcd.lowPowerTimeoutAfterButtonPress += SECONDS_TO_UPTIME(5);
	else
		config.lcd.lowPowerTimeoutAfterButtonPress -= SECONDS_TO_UPTIME(seconds);

	showLcdLowPowerTimeout();
}

static UpdateStatus menuStateEnterReceiverSettings(void)
{
	ENTER_MENU(receiverMenu);
	return menuStateShowCurrentItem();
}

static UpdateStatus menuStateEnterReceiverDebugSettings(void)
{
	ENTER_MENU(receiverDebugMenu);
	return menuStateShowCurrentItem();
}

static UpdateStatus menuStateEnterReceiverResynchronisationIntervalSettings(void)
{
	memcpy(
		menuAdjustmentText,
		RECEIVER_RESYNCHRONISATION_INTERVAL_TEXT,
		sizeof(RECEIVER_RESYNCHRONISATION_INTERVAL_TEXT));

	showReceiverResynchronisationInterval();

	menuUpdate = menuStateShowMultipleAdjustments;
	makeAdjustments[0] = makeReceiverResynchronisationIntervalHoursAdjustment;
	makeAdjustments[1] = makeReceiverResynchronisationIntervalMinutesAdjustment;
	makeAdjustments[2] = NO_CALLBACK;
	return menuStateShowMultipleAdjustments();
}

static void showReceiverResynchronisationInterval(void)
{
	unsigned long hours = config.receiver.resynchronisationUptimeInterval / MINUTES_TO_UPTIME(60);
	unsigned long minutes =
		config.receiver.resynchronisationUptimeInterval % MINUTES_TO_UPTIME(60) / MINUTES_TO_UPTIME(1);

	numberFormattingAsTwoDigits(hours, menuAdjustmentText + RECEIVER_RESYNCHRONISATION_INTERVAL_HH_POSITION);
	numberFormattingAsTwoDigits(minutes, menuAdjustmentText + RECEIVER_RESYNCHRONISATION_INTERVAL_MM_POSITION);
}

static void makeReceiverResynchronisationIntervalHoursAdjustment(void)
{
	unsigned long interval = config.receiver.resynchronisationUptimeInterval + MINUTES_TO_UPTIME(60);
	unsigned long hours = interval / MINUTES_TO_UPTIME(60);

	if (hours > 24)
		interval -= 25 * MINUTES_TO_UPTIME(60);

	config.receiver.resynchronisationUptimeInterval = interval;
	showReceiverResynchronisationInterval();
}

static void makeReceiverResynchronisationIntervalMinutesAdjustment(void)
{
	unsigned long minutes =
		config.receiver.resynchronisationUptimeInterval % MINUTES_TO_UPTIME(60) / MINUTES_TO_UPTIME(1);

	if (minutes + 5 <= 59)
		config.receiver.resynchronisationUptimeInterval += MINUTES_TO_UPTIME(5);
	else
		config.receiver.resynchronisationUptimeInterval -= MINUTES_TO_UPTIME(minutes);

	showReceiverResynchronisationInterval();
}

static UpdateStatus menuStateEnterReceiverDebugShowDecodingSettings(void)
{
	memcpy(menuAdjustmentText, RECEIVER_DEBUG_SHOW_DECODING_TEXT, sizeof(RECEIVER_DEBUG_SHOW_DECODING_TEXT));
	numberFormattingAsYesNo(
		config.receiver.flags.showDecodingDebug,
		menuAdjustmentText + RECEIVER_DEBUG_SHOW_DECODING_POSITION);

	menuUpdate = menuStateShowAdjustment;
	makeAdjustment = makeReceiverDebugShowDecodingAdjustment;
	return menuStateShowAdjustment();
}

static void makeReceiverDebugShowDecodingAdjustment(void)
{
	config.receiver.flags.showDecodingDebug ^= 1;
	numberFormattingAsYesNo(
		config.receiver.flags.showDecodingDebug,
		menuAdjustmentText + RECEIVER_DEBUG_SHOW_DECODING_POSITION);
}

static UpdateStatus menuStateEnterReceiverDebugCarrierOnIoSettings(void)
{
	memcpy(menuAdjustmentText, RECEIVER_DEBUG_CARRIER_ON_IO_TEXT, sizeof(RECEIVER_DEBUG_CARRIER_ON_IO_TEXT));
	numberFormattingAsYesNo(
		config.receiver.flags.showCarrierDetectionDebug,
		menuAdjustmentText + RECEIVER_DEBUG_CARRIER_ON_IO_POSITION);

	menuUpdate = menuStateShowAdjustment;
	makeAdjustment = makeReceiverDebugCarrierOnIoAdjustment;
	return menuStateShowAdjustment();
}

static void makeReceiverDebugCarrierOnIoAdjustment(void)
{
	config.receiver.flags.showCarrierDetectionDebug ^= 1;
	numberFormattingAsYesNo(
		config.receiver.flags.showCarrierDetectionDebug,
		menuAdjustmentText + RECEIVER_DEBUG_CARRIER_ON_IO_POSITION);
}

static UpdateStatus menuStateEnterReceiverDebugCarrierOnBacklightSettings(void)
{
	memcpy(
		menuAdjustmentText,
		RECEIVER_DEBUG_CARRIER_ON_BACKLIGHT_TEXT,
		sizeof(RECEIVER_DEBUG_CARRIER_ON_BACKLIGHT_TEXT));

	numberFormattingAsYesNo(
		config.receiver.flags.showCarrierDetectionDebugFlash,
		menuAdjustmentText + RECEIVER_DEBUG_CARRIER_ON_BACKLIGHT_POSITION);

	menuUpdate = menuStateShowAdjustment;
	makeAdjustment = makeReceiverDebugCarrierOnBacklightAdjustment;
	return menuStateShowAdjustment();
}

static void makeReceiverDebugCarrierOnBacklightAdjustment(void)
{
	config.receiver.flags.showCarrierDetectionDebugFlash ^= 1;
	numberFormattingAsYesNo(
		config.receiver.flags.showCarrierDetectionDebugFlash,
		menuAdjustmentText + RECEIVER_DEBUG_CARRIER_ON_BACKLIGHT_POSITION);
}

static UpdateStatus menuStateEnterExit(void)
{
	ENTER_MENU(exitMenu);
	return menuStateShowCurrentItem();
}

static UpdateStatus menuStateInExitEnterSave(void)
{
	configSave();
	menuUpdate = menuStateTransitionToLeftScreen;
	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus menuStateTransitionToLeftScreen(void)
{
	if (screenTransitionToLeft(NO_CALLBACK))
		menuUpdate = menuStateWaitForTransitionToLeftScreen;

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus menuStateWaitForTransitionToLeftScreen(void)
{
	if (screenUpdate() == UpdateStatus_Idle)
		menuUpdate = menuStateIdle;

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus menuStateInExitEnterDoNotSave(void)
{
	menuUpdate = menuStateTransitionToLeftScreen;
	return UpdateStatus_MoreWorkQueued;
}

void menuEnable(void)
{
	buttonOnShortPress(onButtonShortPress);
	buttonOnLongPress(onButtonLongPress);
}

static void onButtonShortPress(void)
{
	buttonPress = ButtonPress_Short;
}

static void onButtonLongPress(void)
{
	buttonPress = ButtonPress_Long;
}
