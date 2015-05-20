#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926535
#define SAMPLING_FREQUENCY_HZ 47953.17073
#define SINE_FREQUENCY_HZ 60000

static unsigned short doubleToFixed(double value);

int main(int argc, char *argv[])
{
	const int numberOfSamples = 1230;

	int i;
	double sample;
	FILE *fd = fopen("stimulus.txt", "wb");

	if (!fd)
	{
		perror("Unable to open stimulus.txt");
		return 1;
	}

	for (i = 0; i < numberOfSamples; i++)
	{
		sample = sin(2 * PI * SINE_FREQUENCY_HZ * i / SAMPLING_FREQUENCY_HZ);
		fprintf(fd, "0x%.4x\n", doubleToFixed(sample));
	}

	fclose(fd);
	return 0;
}

static unsigned short doubleToFixed(double value)
{
	long longValue = (long) (value * 32768 + 0.5);
	return longValue < -32768
		? -32768
		: longValue > 32767
			? 32767
			: (unsigned short) longValue;
}
