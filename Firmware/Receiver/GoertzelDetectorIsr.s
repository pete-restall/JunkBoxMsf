;
; Counter timings assume an ISR frequency of ~5kHz (79.9220kHz sampling rate, 16 samples per interrupt).
;

	.include "xc.inc"

; Carrier magnitude^2 historic values (when on) for threshold detection:
	.equ CARRIER_HISTORY_POWER2, 4
	.equ CARRIER_HISTORY_SIZE, (1 << CARRIER_HISTORY_POWER2)

; Bit numbers for the ISR state flags:
	.equ FLAG_SHOW_DEBUG_OUTPUT, 7
	.equ FLAG_SHOW_DEBUG_FLASH, 6
	.equ FLAG_CARRIER_ON, 5
	.equ FLAG_CARRIER_DETECT_OFF, 4
	.equ FLAG_EXPECTING_FIRST_SECOND_OF_NEW_MINUTE, 3
	.equ FLAG_EXPECTING_SECOND_MARKER, 2
	.equ FLAG_SYNCHRONISE_CLOCK_AT_FIRST_SECOND_OF_NEW_MINUTE, 1

; Counter values for deciding if the carrier is on or off (the difference
; is hysteresis) plus what constitutes the minute marker:
	.equ CARRIER_OFF_COUNTER_DEFINITELY_OFF, 300
	.equ CARRIER_OFF_COUNTER_DEFINITELY_ON, 200
	.equ CARRIER_OFF_COUNTER_MINUTE_MARKER, 2250

; Counter values for bit sampling intervals:
	.equ BIT_SAMPLE_PERIOD_SECOND_MARKER_TO_A, 300
	.equ BIT_SAMPLE_PERIOD_A_TO_B, 500
	.equ BIT_SAMPLE_PERIOD_A_STOP, 200
	.equ BIT_SAMPLE_PERIOD_B_STOP, 200

; Bit meanings for the _receiverBits variable:
	.equ RECEIVER_BITS_CONSUME, 15
	.equ RECEIVER_BITS_MINUTE_MARKER, 14
	.equ RECEIVER_BITS_SYNCHRONISE_CLOCK_AT_NEXT_MINUTE, 13
	.equ RECEIVER_BITS_CLOCK_SYNCHRONISED, 12

; Pins for showing the carrier detection output:
	.equ LATF_CARRIER_DETECT_BIT, 3
	.equ LATD_CARRIER_DETECT_BIT, 1

; Bit numbers for the clock module's state flags:
	.equ CLOCK_FLAGS_SYNCHRONISE_CLOCK, 4

; Synchronisation value (ie. detection algorithm latency) for the clock's
; timer - 32.768kHz timer is assumed, along with a 5kHz ISR frequency:
	.equ CLOCK_TIMER_VALUE_AT_SYNCHRONISATION, CARRIER_OFF_COUNTER_DEFINITELY_OFF * 32768 / 5000

	.section .goertzelIsrBss, bss, near
	.align 2

; Goertzel algorithm state variables:
q1: .space 2
q2: .space 2
blockRemaining: .space 2

; Carrier threshold detection state variables:
sumCarrierSquaredMagnitudeLow: .space 2
sumCarrierSquaredMagnitudeHigh: .space 2
meanCarrierSquaredMagnitude: .space 2
carrierSquaredMagnitudes: .space 2 * (CARRIER_HISTORY_SIZE - 1)
carrierSquaredMagnitudesLast: .space 2
currentCarrierSquaredMagnitude: .space 2

; MSF bit decoding state variables:
carrierOffCounter: .space 2
minuteSynchronisationCounter: .space 2
bitSamplePeriodCounter: .space 2
bitSampleState: .space 2
bitSampleResult: .space 2

; ISR state variables:
flags: .space 1

	.text
	.extern _memset
	.extern _goertzelBlockSize
	.extern _goertzelCosW
	.extern _goertzelSinW
	.extern _receiverBits
	.extern _clockFlags

;
; void goertzelDetectorIsrInitialise(unsigned int showDebugOutput)
;
; Initialise all ISR state to defaults.  Bit flags in showDebugOutput are:
;
;     0x0001 Show carrier detection result on LATF pin (non-inverted)
;     0x0002 Show carrier detection result on LATD pin (LCD backlight - inverted)
;
	.global _goertzelDetectorIsrInitialise
_goertzelDetectorIsrInitialise:
	; Zero the entire block of memory the ISR uses:
	push W0
	mov #.startof.(.goertzelIsrBss), W0
	clr W1
	mov #.sizeof.(.goertzelIsrBss), W2
	rcall _memset
	pop W0

	; Check configuration settings and cache locally for the ISR to use:
	btsc W0, #0
	bset flags, #FLAG_SHOW_DEBUG_OUTPUT
	btsc W0, #1
	bset flags, #FLAG_SHOW_DEBUG_FLASH

	; Initialise the non-zero elements of the Goertzel state:
	mov _goertzelBlockSize, W0
	mov W0, blockRemaining

	mov #handle(exitIsr), W0
	mov W0, bitSampleState

	mov #carrierSquaredMagnitudes, W0
	mov W0, currentCarrierSquaredMagnitude

	return


;
; void _ADCInterrupt(void)
;
; Automatically wired ISR for ADC conversion.
;
	.global __ADCInterrupt
__ADCInterrupt:
	bclr IFS0, #ADIF
	push.s
	push W4
	push W5
	push W6
	push W7
	push W8
	push W11
	push DCOUNT
	push DOSTARTL
	push DOSTARTH
	push DOENDL
	push DOENDH

	; The number of samples to consume:
startProcessingNewBlockOfSamples:
	mov #16, W0
	sub blockRemaining
	bra NN, startLoopWithRestore
	add blockRemaining, WREG

	; Restore state from previous interrupt:
startLoopWithRestore:
	mov _goertzelCosW, W4
	mov q1, W5
	mov q2, W7
	mov #ADCBUF0, W8

startLoop:
	dec W0, W0
	do W0, endLoop
	lac W7, #0, A                ; A = q2
	neg A                        ; A = -q2
	mac W4 * W5, A, [W8]+=2, W6  ; A = -q2 + cosW * q1, W8 = next ADCBUF address, W6 = sample
	mac W4 * W5, A               ; A = -q2 + 2 * cosW * q1
	add W6, #0, A                ; A = -q2 + 2 * cosW * q1 + sample

	mov W5, W7                   ; q2 = q1
endLoop:
	sac.r A, #1, W5              ; q1 = q0 >> 1

	; If there's more block remaining than we had samples for then we're done:
	neg blockRemaining, WREG
	bra N, decodeCarrierDetectorOutput

	; Compute the magnitude (squared) of the detected signal:
	mpy.n W4 * W7, B             ; B = q2 * -cosW
	add W5, #0, B                ; B = q1 - q2 * cosW
	sac.r B, #0, W5              ; W5 = real (q1 - q2 * cosW)

	mov W4, W1                   ; Save, in case of loop re-entry

	mov _goertzelSinW, W4
	mpy W4 * W7, B               ; B = q2 * sinW
	sac.r B, #0, W4              ; W4 = imaginary (q2 * sinW)

	mpy W4 * W4, B
	mac W5 * W5, B               ; B = real^2 + imaginary^2
	sac.r B, #0, W3              ; W3 = magnitude^2

	mov W1, W4                   ; Restore, in case of loop re-entry

	; Decide whether the output is significantly higher or lower than the mean:
	mov meanCarrierSquaredMagnitude, W5
	lsr W5, #1, W5               ; W5 = 50% of mean magnitude^2
	lsr W5, #1, W2               ; W2 = 25% of mean magnitude^2 (hysteresis)
	add W5, W2, W5               ; W5 = 75% of mean magnitude^2

checkIfOutputIsGreaterThanThreshold:
	cp W3, W5
	bra LTU, checkIfOutputIsLessThanThreshold
	bset flags, #FLAG_CARRIER_ON ; W3 >= high value
	goto addOutputIntoHistory

checkIfOutputIsLessThanThreshold:
	cp W3, W2
	bra GTU, addOutputIntoHistory
	bclr flags, #FLAG_CARRIER_ON ; W3 <= low value - don't add 'off' magnitude^2 to history, otherwise it'll skew
	goto resetStateToPotentiallyReEnterLoop

	; Adjust the sum of the carrier magnitude^2 to include the latest output, while removing the oldest output from
	; the circular buffer:
addOutputIntoHistory:
	mov sumCarrierSquaredMagnitudeLow, W5
	add W5, W3, W5    ; W5 = sumCarrierSquaredMagnitudeLow + magnitude^2
	btsc SR, #C
	inc sumCarrierSquaredMagnitudeHigh

	mov currentCarrierSquaredMagnitude, W1
	mov [W1], W11     ; Retrieve the oldestMagnitude^2 from the circular buffer
	sub W5, W11, W5   ; W5 = sumCarrierSquaredMagnitudeLow + magnitude^2 - oldestMagnitude^2
	btss SR, #C
	dec sumCarrierSquaredMagnitudeHigh
	mov W3, [W1]      ; Remove the oldestMagnitude^2 from the circular buffer - overwrite with magnitude^2
	mov W5, sumCarrierSquaredMagnitudeLow

	; Calculate the mean magnitude^2 by right-shifting the sum to achieve a fast division by 2:
	mov sumCarrierSquaredMagnitudeHigh, W11
	sl W11, #(16 - CARRIER_HISTORY_POWER2), W11
	lsr W5, #CARRIER_HISTORY_POWER2, W5
	ior W11, W5, W5
	mov W5, meanCarrierSquaredMagnitude

	; Adjust the current magnitude^2 pointer - wrap at the end of the history buffer:
	inc2 W1, W1
	mov #carrierSquaredMagnitudesLast, W2
	cp W2, W1
	bra GE, storeIncrementedCurrentOutputPointer
	mov #carrierSquaredMagnitudes, W1

storeIncrementedCurrentOutputPointer:
	mov W1, currentCarrierSquaredMagnitude

	; Reset the block count and re-enter the loop to consume remaining samples:
resetStateToPotentiallyReEnterLoop:
	clr W5     ; q1 = 0
	clr W7     ; q2 = 0
	mov _goertzelBlockSize, W2
	mov W2, blockRemaining
	sub blockRemaining
	cp0 W0
	bra NZ, startLoop

decodeCarrierDetectorOutput:
	; Save the Goertzel algorithm's state:
	mov W5, q1
	mov W7, q2

	; If the receiver module's decoding has a complete minute then we're getting ready to synchronise:
	btsc _receiverBits, #RECEIVER_BITS_SYNCHRONISE_CLOCK_AT_NEXT_MINUTE
	bset flags, #FLAG_SYNCHRONISE_CLOCK_AT_FIRST_SECOND_OF_NEW_MINUTE

	; Rolling count (debounce) of the number of ISR calls where the carrier has been off,
	; then a jump to the right state handler based off that count:
	btss flags, #FLAG_CARRIER_ON
	inc2 carrierOffCounter
	dec carrierOffCounter
	bra NN, L1
	clr carrierOffCounter       ; Limit the carrierOffCounter to minimum

L1:	mov #CARRIER_OFF_COUNTER_DEFINITELY_OFF, W0
	cp carrierOffCounter
	bra GE, carrierIsDefinitelyOff

	mov #CARRIER_OFF_COUNTER_DEFINITELY_ON, W0
	cp carrierOffCounter
	bra LE, carrierIsDefinitelyOn

	btsc flags, #FLAG_CARRIER_DETECT_OFF
	goto carrierIsProbablyOff    ; Previously detected off, so probably still off
	goto carrierIsProbablyOn     ; Previously detected on, so probably still on

carrierIsDefinitelyOff:
	mov W0, carrierOffCounter    ; Limit the carrierOffCounter to maximum
	btsc flags, #FLAG_CARRIER_DETECT_OFF
	goto carrierIsProbablyOff    ; Only do the processing below if the carrier was previously on

carrierIsDefinitelyOffAndWasPreviouslyOn:
	bset flags, #FLAG_CARRIER_DETECT_OFF

	btsc flags, #FLAG_SHOW_DEBUG_OUTPUT
	bclr LATF, #LATF_CARRIER_DETECT_BIT
	btsc flags, #FLAG_SHOW_DEBUG_FLASH
	bset LATD, #LATD_CARRIER_DETECT_BIT

	mov W0, minuteSynchronisationCounter

	; If this isn't the start of a new second or minute then the carrier is probably just off:
	mov #((1 << FLAG_EXPECTING_FIRST_SECOND_OF_NEW_MINUTE) | (1 << FLAG_EXPECTING_SECOND_MARKER)), W0
	and flags, WREG
	bra Z, carrierIsProbablyOff

	; If we need to synchronise the clock with the first second of the new minute, do so:
	.equ FLAG_COMBO_SYNCHRONISE, ((1 << FLAG_EXPECTING_FIRST_SECOND_OF_NEW_MINUTE) | (1 << FLAG_SYNCHRONISE_CLOCK_AT_FIRST_SECOND_OF_NEW_MINUTE))

	mov #FLAG_COMBO_SYNCHRONISE, W0
	and flags, WREG
	mov #FLAG_COMBO_SYNCHRONISE, W1
	cp W0, W1
	bra NZ, noClockSynchronisationRequired

	; TODO: ONLY SYNCHRONISE IF THE NUMBER OF ISR CALLS SINCE THE LAST 'OFF' IS < 7500 OR SO (IE. WE'VE NOT
	; MISSED A SECOND MARKER)

clockSynchronisationRequired:
	mov #CLOCK_TIMER_VALUE_AT_SYNCHRONISATION, W0
	mov W0, TMR1
	bset _receiverBits, #RECEIVER_BITS_CLOCK_SYNCHRONISED
	bset _clockFlags, #CLOCK_FLAGS_SYNCHRONISE_CLOCK
	bclr flags, #FLAG_SYNCHRONISE_CLOCK_AT_FIRST_SECOND_OF_NEW_MINUTE

	; Setup the counters and state for the next iteration:
noClockSynchronisationRequired:
	mov #BIT_SAMPLE_PERIOD_SECOND_MARKER_TO_A, W0
	mov W0, bitSamplePeriodCounter
	mov #handle(waitToSampleBitA), W0
	mov W0, bitSampleState

	bclr flags, #FLAG_EXPECTING_FIRST_SECOND_OF_NEW_MINUTE
	bclr flags, #FLAG_EXPECTING_SECOND_MARKER

carrierIsProbablyOff:
	inc minuteSynchronisationCounter
	goto trySamplingBit

carrierIsDefinitelyOn:
	btss flags, #FLAG_CARRIER_DETECT_OFF
	goto carrierIsProbablyOn    ; Only do the processing below if the carrier was previously off

carrierIsDefinitelyOnAndWasPreviouslyOff:
	bclr flags, #FLAG_CARRIER_DETECT_OFF

	btsc flags, #FLAG_SHOW_DEBUG_OUTPUT
	bset LATF, #LATF_CARRIER_DETECT_BIT
	btsc flags, #FLAG_SHOW_DEBUG_FLASH
	bclr LATD, #LATD_CARRIER_DETECT_BIT

	clr carrierOffCounter

	; Determine what the previous period of carrier 'off' meant:
	mov #CARRIER_OFF_COUNTER_MINUTE_MARKER, W0
	cp minuteSynchronisationCounter
	bra LT, carrierIsProbablyOn    ; Not long enough to be a minute marker
	bset flags, #FLAG_EXPECTING_FIRST_SECOND_OF_NEW_MINUTE

	clr _receiverBits
	bset _receiverBits, #RECEIVER_BITS_MINUTE_MARKER
	bset _receiverBits, #RECEIVER_BITS_CONSUME

carrierIsProbablyOn:
	goto trySamplingBit

trySamplingBit:
	mov bitSampleState, W0
	goto W0

waitToSampleBitA:
	dec bitSamplePeriodCounter
	bra NZ, exitIsr

startSamplingBitA:
	mov #BIT_SAMPLE_PERIOD_A_TO_B, W0
	mov W0, bitSamplePeriodCounter

	clr _receiverBits
	clr bitSampleResult

	mov #handle(sampleBitA), W0
	mov W0, bitSampleState

sampleBitA:
	btss flags, #FLAG_CARRIER_ON
	inc2 bitSampleResult
	dec bitSampleResult

	dec bitSamplePeriodCounter
	mov #BIT_SAMPLE_PERIOD_A_STOP, W0
	cp bitSamplePeriodCounter
	bra GE, exitIsr

stopSamplingBitA:
	mov #handle(waitToSampleBitB), W0
	mov W0, bitSampleState

	btss bitSampleResult, #15
	bset _receiverBits, #0
	goto exitIsr

waitToSampleBitB:
	dec bitSamplePeriodCounter
	bra NZ, exitIsr

startSamplingBitB:
	clr bitSampleResult

	mov #handle(sampleBitB), W0
	mov W0, bitSampleState

	mov #(BIT_SAMPLE_PERIOD_A_TO_B - BIT_SAMPLE_PERIOD_B_STOP), W0
	mov W0, bitSamplePeriodCounter

sampleBitB:
	btss flags, #FLAG_CARRIER_ON
	inc2 bitSampleResult
	dec bitSampleResult

	dec bitSamplePeriodCounter
	bra NZ, exitIsr

stopSamplingBitB:
	mov #handle(exitIsr), W0
	mov W0, bitSampleState

	btss bitSampleResult, #15
	bset _receiverBits, #1
	bset _receiverBits, #RECEIVER_BITS_CONSUME

	bset flags, #FLAG_EXPECTING_SECOND_MARKER

	; Restore all previous state before returning from the ISR:
exitIsr:
	pop DOENDH
	pop DOENDL
	pop DOSTARTH
	pop DOSTARTL
	pop DCOUNT
	pop W11
	pop W8
	pop W7
	pop W6
	pop W5
	pop W4
	pop.s
	retfie

	.end
