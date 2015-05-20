#ifndef __JUNKBOXMSF_NUMBERFORMATTING_H
#define	__JUNKBOXMSF_NUMBERFORMATTING_H

extern void numberFormattingAsTwoDigits(unsigned char number, char *output);
extern void numberFormattingAsFourDigits(unsigned int number, char *output);
extern void numberFormattingAsYesNo(unsigned char number, char *output);

#endif
