#include <xc.h>

#pragma config FPR = XT_PLL16
#pragma config FOS = PRI
#pragma config FCKSMEN = CSW_FSCM_ON
#pragma config FWPSB = WDTPSB_16
#pragma config FWPSA = WDTPSA_512
#pragma config WDT = WDT_OFF
#pragma config FPWRT = PWRT_64
#pragma config BODENV = BORV_45
#pragma config BOREN = PBOR_ON
#pragma config LPOL = PWMxL_ACT_HI
#pragma config HPOL = PWMxH_ACT_HI
#pragma config PWMPIN = RST_IOPIN
#pragma config MCLRE = MCLR_EN
#pragma config GWRP = GWRP_OFF
#pragma config GCP = CODE_PROT_OFF
#pragma config ICS = PGD
