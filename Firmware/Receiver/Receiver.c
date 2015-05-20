#include <xc.h>
#include <string.h>

#include "../Clock.h"
#include "../Config.h"
#include "../Debug.h"
#include "../LowPowerMode.h"
#include "../Mcu.h"
#include "../Menu.h"
#include "../Receiver.h"
#include "../Screen.h"
#include "../Screen/Lcd.h"

#include "GoertzelDetector.h"
#include "MsfDecoder.h"

#if MCU_INSTRUCTION_FREQUENCY_HZ != 29491200ul
#error The values in this file need reviewing as they are calibrated to a different clock frequency !  See DesignCalculations.ods.
#endif

#define ADCON1_SIMSAM_OFF 0
#define ADCON1_SSRC_AUTO_CONVERT 7
#define ADCON1_ASAM_AUTO_START 1
#define ADCON1_FORM_SIGNED_FRACTIONAL 3

#define ADCON2_VCFG_EXTERNAL_REFERENCES 3
#define ADCON2_CHPS_CONVERT_CHANNEL0 0
#define ADCON2_SMPI_INTERRUPT_NUMBER_OF_SAMPLES(x) ((x) - 1)

#define ADCON3_ADCS_CALIBRATED_VALUE 17
#define ADCON3_SAMC_CALIBRATED_VALUE 29

#define ADCHS_CH0SA_HIGHEST_GAIN 4
#define ADCHS_CH0SA_INTERMEDIATE_GAIN 3
#define ADCHS_CH0SA_LOWEST_GAIN 2

#define ADCHS_CHANNEL_TO_STATUS_CHANNEL(channel) \
	((channel) == ADCHS_CH0SA_HIGHEST_GAIN \
		? 3 \
		: (channel) == ADCHS_CH0SA_INTERMEDIATE_GAIN \
			? 2 \
			: (channel) == ADCHS_CH0SA_LOWEST_GAIN \
				? 1 \
				: 0)

#define RECEIVER_DECODING_TIMEOUT_SECONDS (3 * 60)
#define RECEIVER_SYNCHRONISATION_TIMEOUT_SECONDS 10

ReceiverState receiverUpdate;
ReceiverStatus receiverStatus;

volatile union
{
	unsigned int asWord;

	struct
	{
		unsigned int a : 1; /* Bit locations must match in GoertzelDetectorIsr.s, too */
		unsigned int b : 1;
		unsigned int unused : 10;
		unsigned int clockSynchronised : 1;
		unsigned int synchroniseClockAtNextMinute : 1;
		unsigned int minuteMarker : 1;
		unsigned int consume : 1;
	};
} receiverBits;

static unsigned long lastReceiverSynchronisationStartedTimestamp;
static unsigned long lastReceiverDecodingCompletedTimestamp;

static UpdateStatus receiverStateDisabledAtStartup(void);
static UpdateStatus receiverStateIdle(void);
static void receiverEnableOnChannel(unsigned char channel);
static void adcDisableLowPowerMode(void);
static UpdateStatus receiverStateWaitUntilLowPowerModeExited(void);
static UpdateStatus receiverStateStartDecoding(void);
static void transitionToDebugScreen(void);
static UpdateStatus receiverStateDecoding(void);
static void receiverDisableWithNextState(ReceiverState nextState);
static UpdateStatus receiverStateFailedToDecode(void);
static UpdateStatus receiverStateTransitionFromDebugScreen(void);
static UpdateStatus receiverStateEnterLowPowerMode(void);
static void adcEnableLowPowerMode(void);
static UpdateStatus receiverStateWaitUntilLowPowerModeEntered(void);
static UpdateStatus receiverStateWaitUntilSynchronised(void);
static UpdateStatus receiverStateFailedToSynchronise(void);

#if DEBUG_RECEIVER_STUB_DATE_AND_TIME
static UpdateStatus receiverStateDebugStubDateAndTime(void);
#endif

void receiverInitialise(void)
{
	receiverUpdate = receiverStateDisabledAtStartup;
	memset(&receiverStatus, 0, sizeof(ReceiverStatus));
}

static UpdateStatus receiverStateDisabledAtStartup(void)
{
	return UpdateStatus_Idle;
}

static UpdateStatus receiverStateIdle(void)
{
	if (!config.receiver.resynchronisationUptimeInterval)
		return UpdateStatus_Idle;

	if (UPTIME_INTERVAL_SINCE(receiverStatus.lastSynchronisationCompletedTimestamp) <
	    config.receiver.resynchronisationUptimeInterval - SECONDS_TO_UPTIME(10))
		return UpdateStatus_Idle;

	receiverEnable();
	return UpdateStatus_MoreWorkQueued;
}

void receiverEnable(void)
{
	receiverEnableOnChannel(ADCHS_CH0SA_HIGHEST_GAIN);
}

static void receiverEnableOnChannel(unsigned char channel)
{
	adcDisableLowPowerMode();
	lastReceiverSynchronisationStartedTimestamp = UPTIME_NOW;
	receiverUpdate = receiverStateStartDecoding;
	receiverStatus.flagsAsPrimitive = 0;
	receiverStatus.flags.on = 1;
	receiverStatus.channel = ADCHS_CHANNEL_TO_STATUS_CHANNEL(channel);
	receiverBits.asWord = 0;
	goertzelDetectorInitialise();
	msfDecoderInitialise();

	ADCHSbits.CH0SA = channel;
	lowPowerModeDisable();
	receiverUpdate = receiverStateWaitUntilLowPowerModeExited;
}

static void adcDisableLowPowerMode(void)
{
	PMD1bits.ADCMD = 0;
	nop2();

	ADPCFGbits.PCFG5 = 1;
	ADCON1bits.SIMSAM = ADCON1_SIMSAM_OFF;
	ADCON1bits.SSRC = ADCON1_SSRC_AUTO_CONVERT;
	ADCON1bits.ASAM = ADCON1_ASAM_AUTO_START;
	ADCON1bits.FORM = ADCON1_FORM_SIGNED_FRACTIONAL;

	ADCON2bits.VCFG = ADCON2_VCFG_EXTERNAL_REFERENCES;
	ADCON2bits.CHPS = ADCON2_CHPS_CONVERT_CHANNEL0;
	ADCON2bits.SMPI = ADCON2_SMPI_INTERRUPT_NUMBER_OF_SAMPLES(16);

	ADCON3bits.SAMC = ADCON3_SAMC_CALIBRATED_VALUE;
	ADCON3bits.ADCS = ADCON3_ADCS_CALIBRATED_VALUE;

	ADCHSbits.CH0SA = ADCHS_CH0SA_HIGHEST_GAIN;

	IFS0bits.ADIF = 0;
	IEC0bits.ADIE = 1;
}

static UpdateStatus receiverStateWaitUntilLowPowerModeExited(void)
{
	if (lowPowerModeIsEnabled())
		return UpdateStatus_MoreWorkQueued;

	receiverUpdate = receiverStateStartDecoding;

#if !DEBUG_RECEIVER_STUB_DATE_AND_TIME
	ADCON1bits.ADON = 1;
#endif

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus receiverStateStartDecoding(void)
{
	if (!menuIsVisible && config.receiver.flags.showDecodingDebug)
		screenPutStringsOnRightBothLines("MSF Sync Started", " [ Debug Mode ] ", transitionToDebugScreen);

	receiverUpdate = receiverStateDecoding;

	return UpdateStatus_MoreWorkQueued;
}

static void transitionToDebugScreen(void)
{
	screenTransitionToRight(NO_CALLBACK);
}

static UpdateStatus receiverStateDecoding(void)
{
#if !DEBUG_RECEIVER_STUB_DATE_AND_TIME
	if (UPTIME_SECONDS_SINCE(lastReceiverSynchronisationStartedTimestamp) >= RECEIVER_DECODING_TIMEOUT_SECONDS)
	{
		receiverDisableWithNextState(receiverStateFailedToDecode);
		receiverStatus.flags.decoding = 0;
		return UpdateStatus_MoreWorkQueued;
	}

	if (!receiverBits.consume)
		return UpdateStatus_MoreWorkQueued;

	receiverBits.consume = 0;
	if (receiverBits.minuteMarker)
	{
		receiverStatus.flags.decoding = 1;
		msfDecoderMinuteMarker();
	}
	else if (msfDecoderPushBits(receiverBits.a ? ~0 : 0, receiverBits.b ? ~0 : 0))
	{
		receiverStatus.flags.decoding = 0;
		receiverBits.synchroniseClockAtNextMinute = 1;
		receiverUpdate = receiverStateWaitUntilSynchronised;
		lastReceiverDecodingCompletedTimestamp = UPTIME_NOW;
	}
#else
	receiverUpdate = receiverStateDebugStubDateAndTime;
#endif

	return UpdateStatus_MoreWorkQueued;
}

static void receiverDisableWithNextState(ReceiverState nextState)
{
	ADCON1bits.ADON = 0;
	receiverUpdate = nextState;
	receiverStatus.flags.on = 0;
	receiverStatus.flags.decoding = 0;
	receiverStatus.flags.waitingForSynchronisation = 0;
	receiverStatus.channel = 0;

	if (config.receiver.flags.showCarrierDetectionDebugFlash)
		lcdBacklightOff();
}

static UpdateStatus receiverStateFailedToDecode(void)
{
	receiverStatus.flags.failedToSynchronise = 1;
	receiverStatus.numberOfConsecutiveFailedSynchronisations++;

	if (ADCHSbits.CH0SA == ADCHS_CH0SA_HIGHEST_GAIN)
	{
		receiverEnableOnChannel(ADCHS_CH0SA_INTERMEDIATE_GAIN);
	}
	else if (ADCHSbits.CH0SA == ADCHS_CH0SA_INTERMEDIATE_GAIN)
	{
		receiverEnableOnChannel(ADCHS_CH0SA_LOWEST_GAIN);
	}
	else
	{
		receiverStatus.lastSynchronisationCompletedTimestamp = UPTIME_NOW;
		receiverUpdate = receiverStateTransitionFromDebugScreen;
	}

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus receiverStateTransitionFromDebugScreen(void)
{
	if (menuIsVisible || screenTransitionToLeft(NO_CALLBACK))
		receiverUpdate = receiverStateEnterLowPowerMode;

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus receiverStateEnterLowPowerMode(void)
{
	adcEnableLowPowerMode();
	lowPowerModeEnable();
	receiverUpdate = receiverStateWaitUntilLowPowerModeEntered;
	return UpdateStatus_MoreWorkQueued;
}

static void adcEnableLowPowerMode(void)
{
	ADCON1bits.ADON = 0;
	PMD1bits.ADCMD = 1;
	nop2();
}

static UpdateStatus receiverStateWaitUntilLowPowerModeEntered(void)
{
	if (lowPowerModeIsEnabled())
		receiverUpdate = receiverStateIdle;

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus receiverStateWaitUntilSynchronised(void)
{
	receiverStatus.flags.waitingForSynchronisation = 1;
	if (!(receiverBits.clockSynchronised))
	{
		if (UPTIME_SECONDS_SINCE(lastReceiverDecodingCompletedTimestamp) >= RECEIVER_SYNCHRONISATION_TIMEOUT_SECONDS)
			receiverDisableWithNextState(receiverStateFailedToSynchronise);

		return UpdateStatus_MoreWorkQueued;
	}

	receiverDisable();
	receiverStatus.flags.failedToSynchronise = 0;
	receiverStatus.flags.synchronised = 1;
	receiverStatus.numberOfConsecutiveFailedSynchronisations = 0;
	receiverStatus.lastSynchronisationCompletedTimestamp = UPTIME_NOW;

	return UpdateStatus_MoreWorkQueued;
}

static UpdateStatus receiverStateFailedToSynchronise(void)
{
	return receiverStateFailedToDecode();
}

void receiverDisable(void)
{
	receiverDisableWithNextState(
		config.receiver.flags.showDecodingDebug
			? receiverStateTransitionFromDebugScreen
			: receiverStateEnterLowPowerMode);
}

#if DEBUG_RECEIVER_STUB_DATE_AND_TIME
static UpdateStatus receiverStateDebugStubDateAndTime(void)
{
	debugReceiverStubDateAndTime();
	receiverBits.clockSynchronised = 1;
	return receiverStateWaitUntilSynchronised();
}
#endif
