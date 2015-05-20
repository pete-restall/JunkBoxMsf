#include <xc.h>
#include <string.h>

#include "../Clock.h"
#include "../Config.h"

#define CONFIG_CURRENT_VERSION 0x01

extern void eepromReadPage(unsigned int page, void *data);
extern void eepromWritePage(void *data, unsigned int page);

static int readLatestValues(void);
static unsigned long calculateChecksum(const void *data, unsigned int count);

Configuration config;
static unsigned int nextFreePage;

void configInitialise(void)
{
	if (readLatestValues())
		return;

	memset(&config, 0, sizeof(Configuration));
	config.version = CONFIG_CURRENT_VERSION;
	config.receiver.resynchronisationUptimeInterval = MINUTES_TO_UPTIME(10);
	config.lcd.contrast = 3;
	config.lcd.backlightSecondsAfterButtonPress = 5;
}

static int readLatestValues(void)
{
	unsigned int page, latestPage = 0;
	unsigned long latestCounter = 0;
	unsigned long checksum = 0;

	for (page = 0; page < EEPROM_SIZE_IN_PAGES; page++)
	{
		eepromReadPage(page, &config);
		if (config.version == CONFIG_CURRENT_VERSION && config.counter > latestCounter)
		{
			checksum =
				calculateChecksum(&config.asEepromPageWithoutChecksum, sizeof(config.asEepromPageWithoutChecksum));

			if (checksum == config.checksum)
			{
				latestPage = page;
				latestCounter = config.counter;
			}
		}
	}

	if (!latestCounter)
	{
		nextFreePage = 0;
		return 0;
	}

	eepromReadPage(latestPage, &config);
	checksum = calculateChecksum(&config.asEepromPageWithoutChecksum, sizeof(config.asEepromPageWithoutChecksum));

	nextFreePage = latestPage + 1;
	if (nextFreePage >= EEPROM_SIZE_IN_PAGES)
		nextFreePage = 0;

	return checksum == config.checksum;
}

static unsigned long calculateChecksum(const void *data, unsigned int count)
{
	unsigned long checksum = 0;
	while (count-- > 0)
		checksum += *((unsigned char *) data++);

	return checksum;
}

void configSave(void)
{
	unsigned short writeToPage = nextFreePage++;
	if (nextFreePage >= EEPROM_SIZE_IN_PAGES)
		nextFreePage = 0;

	config.counter++;
	config.checksum = calculateChecksum(config.asEepromPageWithoutChecksum, sizeof(config.asEepromPageWithoutChecksum));

	eepromWritePage(&config, writeToPage);
}
