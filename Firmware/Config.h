#ifndef __JUNKBOXMSF_CONFIG_H
#define	__JUNKBOXMSF_CONFIG_H

#define EEPROM_PAGE_SIZE_IN_WORDS 16
#define EEPROM_PAGE_SIZE_IN_BYTES (EEPROM_PAGE_SIZE_IN_WORDS * 2)
#define EEPROM_SIZE_IN_PAGES (1024 / EEPROM_PAGE_SIZE_IN_BYTES)

typedef union
{
	struct
	{
		union
		{
			struct
			{
				unsigned char version;
				unsigned long counter;

				struct
				{
					unsigned char contrast;
					unsigned char backlightSecondsAfterButtonPress;
					unsigned long lowPowerTimeoutAfterButtonPress;
				} lcd;

				struct
				{
					struct
					{
						unsigned int showDecodingDebug : 1;
						unsigned int showCarrierDetectionDebug : 1;
						unsigned int showCarrierDetectionDebugFlash : 1;
					} flags;

					unsigned long resynchronisationUptimeInterval;
				} receiver;
			};

			unsigned int asEepromPageWithoutChecksum[EEPROM_PAGE_SIZE_IN_WORDS - 2];
		};

		unsigned long checksum;
	};

	unsigned int asEepromPage[EEPROM_PAGE_SIZE_IN_WORDS];
} Configuration;

extern Configuration config;

extern void configInitialise(void);
extern void configSave(void);

#endif
