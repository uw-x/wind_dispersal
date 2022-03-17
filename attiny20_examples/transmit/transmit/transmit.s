
;--------------------------------
;------  Transmit Function ------
;--------------------------------

;chip specific
#include <avr/io.h>

;defines
#define BIT_COUNTER r21
#define BYTE_COUNTER r25
#define MASK r20

;------ PACKET DATA ------;

#define PACKET_START r24 ;define the packet start address ----These registers handle the arguments 
#define PACKET_LENGTH r22     ;define the packet length   ----to the TRANSMIT() call

.global TRANSMIT  ;so that it can be used in c files

TRANSMIT:
		sbi DDRA, 7 ;enable PORTA7 as an output
	
TIMER_SETUP:
		ldi r18, 0b00010010     ;COM0B0 and WGM01: toggle COM0B on compare match, CTC mode
		out TCCR0A, r18
		ldi r18, 0b00000000     ; top = 0
		out OCR0A, r18
		out OCR0B, r18          ; toggle at 0

		mov BYTE_COUNTER, PACKET_LENGTH
		ldi XH, 0x00
		mov XL, PACKET_START
TX_LOOP:
		ldi BIT_COUNTER, 8    ;bit counter
		ld r18, X+            ;load the first byte
BYTE_LOOP:
		ldi MASK, 0b10000000  ;create mask
		and MASK, r18         ;r20 gets masked r16
		breq TX_OFF           ;turn the transmitter off
TX_ON:
		sbi TCCR0B, 0          ;turn on the transmitter
		rjmp SHIFT            ;skip TX_OFF
TX_OFF:
		cbi TCCR0B, 0          ;turn off the transmitter
		nop                   ;to make 1s and 0s take the same number of instructions
SHIFT:  
		rcall PAUSE           ;wait to complete one cycle
		dec BIT_COUNTER       ;one bit done
		breq BYTE_DONE        ;is the byte done?
		lsl r18               ;shift the data (if the byte is not done)
		nop                   ;delays for constant 15 clock cycles per bit
		nop
		nop
		nop
		rjmp BYTE_LOOP        ;next bit

BYTE_DONE:
		dec BYTE_COUNTER      ;finished byte
		brne TX_LOOP          ;next byte?
		
		cbi TCCR0B, 0         ;turn off the transmitter
		cbi TCCR0A, 4         ;turn off toggle on compare match
		cbi PORTA,7           ;turn off the transmitter
		ret                   ;done

;;32kbps @ 4MHz
;PAUSE:	
;		ldi  r23, 34          ;delay value: this sets the bitrate (use an online calculator)
;L1:	dec  r23              ;decrease delay by one
;		brne L1               ;is r23 not cleared?
;		ret                   ;done waiting

;;32kbps @ 8 MHz
;PAUSE:     
;		ldi  r23, 75         ;delay value: this sets the bitrate
;L1:		dec  r23             ;decrease the delay by one
;		brne L1              ;is r23 not cleared?
;		nop             ;skip one (for a specific delay)
;       nop
;		ret                  ;done waiting


;4.8 kbps @8MHz
PAUSE:
    ldi  r23, 3
    ldi  r28, 24
L1: dec  r28
    brne L1
    dec  r23
    brne L1
	ret
