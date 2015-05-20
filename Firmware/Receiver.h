#ifndef __JUNKBOXMSF_RECEIVER_H
#define	__JUNKBOXMSF_RECEIVER_H
#include "JunkBoxMsf.h"

typedef UpdateStatus (*ReceiverState)(void);

typedef struct
{
	union
	{
		struct
		{
			unsigned int on : 1;
			unsigned int decoding : 1;
			unsigned int waitingForSynchronisation : 1;
			unsigned int synchronised : 1;
			unsigned int failedToSynchronise : 1;
		} flags;

		unsigned char flagsAsPrimitive;
	};

	unsigned char channel;
	unsigned int numberOfConsecutiveFailedSynchronisations;
	unsigned long lastSynchronisationCompletedTimestamp;
} ReceiverStatus;

extern ReceiverStatus receiverStatus;

extern void receiverInitialise(void);
extern void receiverEnable(void);
extern ReceiverState receiverUpdate;
extern void receiverDisable(void);

#endif
