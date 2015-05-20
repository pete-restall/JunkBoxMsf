#include "NumberFormatting.h"

void numberFormattingAsTwoDigits(unsigned char number, char *output)
{
	unsigned char highDigit = number / 10;
	unsigned char lowDigit = number % 10;
	*(output++) = '0' + highDigit;
	*output = '0' + lowDigit;
}

void numberFormattingAsFourDigits(unsigned int number, char *output)
{
	unsigned char thousands = (unsigned char) (number / 1000), hundreds, tens, units;
	number -= thousands * 1000;
	hundreds = (unsigned char) (number / 100);
	number -= hundreds * 100;
	tens = (unsigned char) (number / 10);
	units = (unsigned char) (number - tens * 10);

	*(output++) = '0' + thousands;
	*(output++) = '0' + hundreds;
	*(output++) = '0' + tens;
	*output = '0' + units;
}

void numberFormattingAsYesNo(unsigned char number, char *output)
{
	if (number)
	{
		*(output++) = 'Y';
		*(output++) = 'e';
		*output = 's';
	}
	else
	{
		*(output++) = 'N';
		*(output++) = 'o';
		*output = ' ';
	}
}
