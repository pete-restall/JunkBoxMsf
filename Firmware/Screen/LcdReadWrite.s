;
; CPU timings assume (maximum) instruction clock of 30MIPS, or a low power mode of around 1.8MIPS.
;
; LCD timings are based on worst case combination of ST7066 and HD44780 datasheet values.
;
	.equ LCD_LATE_DATA_MASK, 0x000f
	.equ LCD_LATE_RS_BIT, 4
	.equ LCD_LATE_RS_MASK, 1 << LCD_LATE_RS_BIT
	.equ LCD_LATE_RW_BIT, 8
	.equ LCD_LATE_RW_MASK, 1 << LCD_LATE_RW_BIT
	.equ LCD_LATD_CLOCK_BIT, 0

	.text
	.extern _lcdLowPowerDelayScale

;
; unsigned char lcdReadBusyFlags(void)
;
; The LCD Busy Flag is bit 7, DDRAM Address is in bits 6-0.
; Execution takes about 2.112us at maximum 30MIPS, including call / return.
;
	.global _lcdReadBusyFlags
_lcdReadBusyFlags:
	bset LATE, #LCD_LATE_RW_BIT
	bclr LATE, #LCD_LATE_RS_BIT

	; Data lines as inputs (> 40ns after RS / RW):
	mov #LCD_LATE_DATA_MASK, W0
	ior TRISE, WREG
	mov W0, TRISE

	; Clock up (> 40ns after RS / RW):
	bset LATD, #LCD_LATD_CLOCK_BIT

	; Burn > 175ns until the data lines become valid:
	mov _lcdLowPowerDelayScale, W3
	mov #1, W4
	lsr W4, W3, W4
	repeat W4
	nop
	mov PORTE, W1

	; Shift / mask W1 so that it is the high nibble:
	mov #0x00f0, W2
	sl W1, #4, W1
	and W1, W2, W1 ; W1 is high nibble - shift / mask while wasting time

	; Clock down (> 230ns after clock up):
	bclr LATD, #LCD_LATD_CLOCK_BIT
	mov #16, W4
	lsr W4, W3, W4
	repeat W4
	nop

	; Clock up for second nibble (> 1000ns after first clock up):
	bset LATD, #LCD_LATD_CLOCK_BIT

	; Burn > 175ns until the data lines become valid:
	mov #1, W4
	lsr W4, W3, W4
	repeat W4
	nop
	mov #LCD_LATE_DATA_MASK, W0
	and PORTE, WREG
	ior W0, W1, W0

	; Clock down (> 230ns after clock up):
	bclr LATD, #LCD_LATD_CLOCK_BIT

	; For a back-to-back call, ensure the caller cannot trigger another clock
	; up < 1000ns after the last one (31 cycles including call / return).
	; The result is in W0:
	mov #8, W4
	lsr W4, W3, W4
	repeat W4
	nop

	return

;
; void lcdWriteRegister(unsigned char data)
;
; Write 8-bits of data to the LCD Instruction Register.
; Execution takes about 2.112us at maximum 30MIPS, including call / return.
;
	.global _lcdWriteRegister
_lcdWriteRegister:
	bclr LATE, #LCD_LATE_RS_BIT
	goto lcdWrite

;
; void lcdWriteMemory(unsigned char data)
;
; Write 8-bits of data to the LCD DDRAM.
; Execution takes about 2.112us at maximum 30MIPS, including call / return.
;
	.global _lcdWriteMemory
_lcdWriteMemory:
	bset LATE, #LCD_LATE_RS_BIT
	nop
	nop

lcdWrite:
	; Assert the RS and (not) RW lines:
	bclr LATE, #LCD_LATE_RW_BIT

	; Data lines as outputs and clock up (> 40ns after RS / RW):
	mov #~LCD_LATE_DATA_MASK, W1
	mov TRISE, W2
	bset LATD, #LCD_LATD_CLOCK_BIT
	and W1, W2, W1
	mov W1, TRISE

	; High nibble output:
	mov #~LCD_LATE_DATA_MASK, W2
	mov LATE, W1
	and W1, W2, W1
	lsr W0, #4, W2
	and #0x0f, W2
	ior W1, W2, W1
	mov W1, LATE

	; Burn some time to allow the data lines to stabilise:
	mov _lcdLowPowerDelayScale, W3
	mov #1, W4
	lsr W4, W3, W4
	repeat W4
	nop

	; Clock down (> 80ns after nibble output stabilises, > 230ns after clock
	; up):
	bclr LATD, #LCD_LATD_CLOCK_BIT

	; Clock up for second nibble (> 1000ns after first clock up):
	mov #13, W4
	lsr W4, W3, W4
	repeat W4
	nop
	bset LATD, #LCD_LATD_CLOCK_BIT

	; Low nibble output:
	mov #~LCD_LATE_DATA_MASK, W2
	and W1, W2, W1
	and #0x0f, W0
	ior W0, W1, W0
	mov W0, LATE

	; Burn some time to allow the data lines to stabilise:
	mov #2, W4
	lsr W4, W3, W4
	repeat W4
	nop

	; Clock down (> 80ns after nibble output stabilises, > 230ns after clock
	; up):
	bclr LATD, #LCD_LATD_CLOCK_BIT

	; For a back-to-back call (either a read or write), ensure the caller
	; cannot trigger another clock-up < 1000ns after the last one (the repeat
	; ensures 32 cycles for write-write, or 31 for write-read, including
	; call / return):
	mov #5, W4
	lsr W4, W3, W4
	repeat W4
	nop

	return

	.end
