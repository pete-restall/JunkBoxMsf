#include <xc.h>
#include <string.h>

#include "../Config.h"
#include "../Mcu.h"

#include "GoertzelDetector.h"

#if MCU_INSTRUCTION_FREQUENCY_HZ != 29491200ul
#error The values in this file (and GoertzelDetectorIsr.s) need reviewing as they are calibrated \
to a different clock frequency !  See DesignCalculations.ods.
#endif

#define CORCON_US_SIGNED_MULTIPLICATION 0
#define CORCON_RND_CONVERGENT_ROUNDING 0
#define CORCON_SATA_DISABLE_SATURATION 0
#define CORCON_SATB_DISABLE_SATURATION 0
#define CORCON_ACCSAT_NORMAL_SATURATION 0

extern void goertzelDetectorIsrInitialise(unsigned int showDebugOutput);

unsigned int goertzelBlockSize;
unsigned int goertzelCosW;
unsigned int goertzelSinW;

void goertzelDetectorInitialise(void)
{
	CORCONbits.US = CORCON_US_SIGNED_MULTIPLICATION;
	CORCONbits.RND = CORCON_RND_CONVERGENT_ROUNDING;
	CORCONbits.ACCSAT = CORCON_ACCSAT_NORMAL_SATURATION;
	CORCONbits.SATA = CORCON_SATA_DISABLE_SATURATION;
	CORCONbits.SATB = CORCON_SATB_DISABLE_SATURATION;

	LATFbits.LATF3 = 0;
	TRISFbits.TRISF3 = 0;

	goertzelBlockSize = 345;
	goertzelCosW = 0x0095;
	goertzelSinW = 0x8000;
	goertzelDetectorIsrInitialise(
		(config.receiver.flags.showCarrierDetectionDebug ? 0x0001 : 0x0000) |
		(config.receiver.flags.showCarrierDetectionDebugFlash ? 0x0002 : 0x0000));
}
