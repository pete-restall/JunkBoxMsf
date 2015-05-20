	.include "xc.inc"

	.equ EEPROM_TBLPAG_ADDRESS, 0x7f
	.equ EEPROM_START_ADDRESS, 0xfc00
	.equ EEPROM_PAGE_SIZE_BYTES_POWER2, 5
	.equ EEPROM_PAGE_SIZE_WORDS, 1 << (EEPROM_PAGE_SIZE_BYTES_POWER2 - 1)
	.equ SR_DISABLE_INTERRUPTS, 0x00e0
	.equ NVMCON_ERASE_FULL_PAGE, 0x4045
	.equ NVMCON_WRITE_FULL_PAGE, 0x4005

	.text
;
; void eepromReadPage(unsigned int page, void *data)
;
; Read a full page of EEPROM into 'data'.
;
	.global _eepromReadPage
_eepromReadPage:
	mov #EEPROM_START_ADDRESS, W2
	sl W0, #EEPROM_PAGE_SIZE_BYTES_POWER2, W0
	add W0, W2, W0

	mov #EEPROM_TBLPAG_ADDRESS, W2
	mov W2, TBLPAG

	push RCOUNT
	repeat #(EEPROM_PAGE_SIZE_WORDS - 1)
	tblrdl [W0++], [W1++]
	pop RCOUNT

	return

;
; void eepromWritePage(void *data, unsigned int page)
;
; Write a full page of EEPROM from 'data'.
;
	.global _eepromWritePage
_eepromWritePage:
	mov W0, W3

	; Convert the EEPROM page number into an EEPROM address and setup for a full page erasure:
	mov #EEPROM_START_ADDRESS, W2
	sl W1, #EEPROM_PAGE_SIZE_BYTES_POWER2, W1
	add W1, W2, W1

	mov #EEPROM_TBLPAG_ADDRESS, W2
	mov W2, TBLPAG
	mov W2, NVMADRU
	mov W1, NVMADR

	mov #NVMCON_ERASE_FULL_PAGE, W2
	mov W2, NVMCON

	rcall unlockEepromAndWrite

	; Write the entire page:
	push RCOUNT
	repeat #(EEPROM_PAGE_SIZE_WORDS - 1)
	tblwtl [W3++], [W1++]
	pop RCOUNT

	mov #NVMCON_WRITE_FULL_PAGE, W2
	mov W2, NVMCON

unlockEepromAndWrite:
	; Disable interrupts during the magic key entry:
	push SR
	mov #SR_DISABLE_INTERRUPTS, W0
	ior SR

	; Magic key entry followed by setting the write flag and re-enabling of interrupts:
	mov #0x55, W0
	mov W0, NVMKEY
	mov #0xaa, W0
	mov W0, NVMKEY
	bset NVMCON, #WR
	pop SR

waitForWriteToFinish:
	btsc NVMCON, #WR
	bra waitForWriteToFinish

	return

	.end
