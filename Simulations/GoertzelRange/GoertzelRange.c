#include <stdio.h>
#include <math.h>

#define PI 3.1415926535
#define BLOCK_SIZE 345
#define COS_W 0x0095
#define SIN_W 0x8000
#define DETECT_FREQUENCY_HZ 60000
#define CLOSE_TO_DETECT_FREQUENCY_HZ (DETECT_FREQUENCY_HZ + 100)
#define SHIFT_TO_ACCUMULATOR(x) (((long) (x)) << 16)
#define SHIFT_FROM_ACCUMULATOR(x) ((x) >> 16)

typedef struct
{
	int blockSize;
	short cosW;
	short sinW;
	int scale;
} GoertzelParameters;

typedef short (*NextSample)(int n);

static void executeLoop(
	const char *testName,
	const GoertzelParameters *parameters,
	NextSample nextSample);

static short highestSample(int n);
static short lowestSample(int n);
static short alternateHighestLowestSample(int n);
static short zero(int n);
static short fullScaleSine(int n);
static short doubleToFixed(double value);
static short fullScaleCosine(int n);
static short closeToFrequencyFullScaleSine(int n);
static short closeToFrequencyFullScaleCosine(int n);
static short smallScaleSine(int n);
static short smallScaleCosine(int n);

int main(int argc, char *argv[])
{
	GoertzelParameters parameters;
	parameters.blockSize = BLOCK_SIZE;
	parameters.cosW = COS_W;
	parameters.sinW = SIN_W;
	parameters.scale = 1;

	executeLoop("Highest Sample", &parameters, highestSample);
	executeLoop("Lowest Sample", &parameters, lowestSample);
	executeLoop("Alternate Highest / Lowest Sample", &parameters, alternateHighestLowestSample);
	executeLoop("Zero Sample", &parameters, zero);
	executeLoop("Full Scale Sine", &parameters, fullScaleSine);
	executeLoop("Full Scale Cosine", &parameters, fullScaleCosine);
	executeLoop("Full Scale Sine (Near)", &parameters, closeToFrequencyFullScaleSine);
	executeLoop("Full Scale Cosine (Near)", &parameters, closeToFrequencyFullScaleCosine);
	executeLoop("Small Scale Sine", &parameters, smallScaleSine);
	executeLoop("Small Scale Cosine", &parameters, smallScaleCosine);

	return 0;
}

static void executeLoop(
	const char *testName,
	const GoertzelParameters *parameters,
	NextSample nextSample)
{
	int i;
	short sample;
	long q0 = 0;
	long minQ = 0, maxQ = 0;
	short q1 = 0, q2 = 0;
	long real, imaginary;

	printf("--- START: %s ---\n", testName);
	for (i = 0; i < parameters->blockSize; i++)
	{
		sample = nextSample(i);
		q0 = 2 * parameters->cosW * q1 -
			SHIFT_TO_ACCUMULATOR(q2) +
			SHIFT_TO_ACCUMULATOR(sample);

		q0 >>= parameters->scale;
		q2 = q1;
		q1 = SHIFT_FROM_ACCUMULATOR(q0);

		if (q0 < minQ)
			minQ = q0;

		if (q0 > maxQ)
			maxQ = q0;
	}

	real = SHIFT_FROM_ACCUMULATOR(
		SHIFT_TO_ACCUMULATOR(q1) - q2 * parameters->cosW);

	imaginary = SHIFT_FROM_ACCUMULATOR(q2 * parameters->sinW);

	printf("Range of Q: %ld to %ld\n", minQ, maxQ);
	printf("Real, Imaginary: %ld, %ld\n", real, imaginary);
	printf("Magnitude^2: %ld\n", real * real + imaginary * imaginary);
	printf("---- END: %s ----\n\n", testName);
}

static short highestSample(int n)
{
	return 0x7fc0;
}

static short lowestSample(int n)
{
	return 0xffc0;
}

static short alternateHighestLowestSample(int n)
{
	return (n % 2 == 0) ? highestSample(n) : lowestSample(n);
}

static short zero(int n)
{
	return 0;
}

static short fullScaleSine(int n)
{
	return doubleToFixed(sin(2 * PI * DETECT_FREQUENCY_HZ * n / BLOCK_SIZE));
}

static short doubleToFixed(double value)
{
	long intValue = (long) 32768 * value + 0.5;
	return intValue > 32767
		? 32767
		: intValue < -32768
			? -32768
			: (short) intValue;
}

static short fullScaleCosine(int n)
{
	return doubleToFixed(cos(2 * PI * DETECT_FREQUENCY_HZ * n / BLOCK_SIZE));
}

static short closeToFrequencyFullScaleSine(int n)
{
	return doubleToFixed(sin(2 * PI * CLOSE_TO_DETECT_FREQUENCY_HZ * n / BLOCK_SIZE));
}

static short closeToFrequencyFullScaleCosine(int n)
{
	return doubleToFixed(cos(2 * PI * CLOSE_TO_DETECT_FREQUENCY_HZ * n / BLOCK_SIZE));
}

static short smallScaleSine(int n)
{
	return doubleToFixed((20.0 / 1024) * sin(2 * PI * DETECT_FREQUENCY_HZ * n / BLOCK_SIZE));
}

static short smallScaleCosine(int n)
{
	return doubleToFixed((20.0 / 1024) * cos(2 * PI * DETECT_FREQUENCY_HZ * n / BLOCK_SIZE));
}
