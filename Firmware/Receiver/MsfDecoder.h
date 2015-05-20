#ifndef __JUNKBOXMSF_RECEIVER_MSFDECODER_H
#define __JUNKBOXMSF_RECEIVER_MSFDECODER_H

extern void msfDecoderInitialise(void);
extern void msfDecoderMinuteMarker(void);
extern int msfDecoderPushBits(unsigned char a, unsigned char b);

#endif
