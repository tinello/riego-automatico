PROCESSOR 16F819
#include <xc.inc>


; CONFIG
CONFIG  FOSC = INTOSCIO       ; Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
CONFIG  WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled)
CONFIG  PWRTE = ON            ; Power-up Timer Enable bit (PWRT enabled)
CONFIG  MCLRE = OFF           ; RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
CONFIG  BOREN = OFF           ; Brown-out Reset Enable bit (BOR disabled)
CONFIG  LVP = OFF             ; Low-Voltage Programming Enable bit (RB3/PGM pin has digital I/O function, HV on MCLR must be used for programming)
CONFIG  CPD = OFF             ; Data EE Memory Code Protection bit (Code protection off)
CONFIG  WRT = OFF             ; Flash Program Memory Write Enable bits (Write protection off)
;CONFIG  CCPMX = RB2           ; CCP1 Pin Selection bit (CCP1 function on RB2)
CONFIG  CP = OFF              ; Flash Program Memory Code Protection bit (Code protection off)



;Constantes
;Registros banco 0
STATUS_REG	EQU	0x03
ADCON0_REG	EQU	0x1F
PORTA_REG	EQU	0x05
PORTB_REG	EQU	0x06
	
;Registros banco 1
OSCCON_REG	EQU	0x8F
TRISA_REG	EQU	0x85
TRISB_REG	EQU	0x86
ADCON1_REG	EQU	0x9F

;STATUS Bits
status_IRP	EQU	0x07
status_RP1	EQU	0x06
status_RP0	EQU	0x05

	    
retardo1	EQU	0x20
retardo2	EQU	0x21
retardo3	EQU	0x22
retardo4	EQU	0x23
	
	

;************* banco 0 *************
bank0	    MACRO
	    bcf		STATUS_REG, status_IRP
	    bcf		STATUS_REG, status_RP1
	    bcf		STATUS_REG, status_RP0
	    ENDM

;************* banco 1 *************
bank1	    MACRO
	    bcf		STATUS_REG, status_IRP
	    bcf		STATUS_REG, status_RP1
	    bsf		STATUS_REG, status_RP0
	    ENDM

;************* banco 2 *************
bank2	    MACRO
	    bsf		STATUS_REG, status_IRP
	    bsf		STATUS_REG, status_RP1
	    bcf		STATUS_REG, status_RP0
	    ENDM

;************* banco 3 *************
bank3	    MACRO
	    bsf		STATUS, status_IRP
	    bsf		STATUS, status_RP1
	    bsf		STATUS, status_RP0
	    ENDM
;-------------------------------------------


	
	
	
PSECT	code, delta=2, abs
ORG	0x0000
resetVector:
	goto	setup

	
PSECT	code, delta=2, abs
ORG	0x000A
setup:
conf_internal_clock_8mhz:
	bank1
	movlw	0x74
        movwf	OSCCON_REG
	
	movlw	0xFF
        movwf	TRISA_REG
	movlw	0x00
        movwf	TRISB_REG
	
	movlw	0x06
        movwf	ADCON1_REG
	
	bank0
	movlw	0x06
        movwf	ADCON0_REG
	
	//Puerto B apagado
	movlw	0x00
        movwf	PORTB_REG
	
	
main:
	btfsc	PORTA_REG, 0x01
	goto	main
	call	sleep_1_tenth
	btfsc	PORTA_REG, 0x01
	goto	main
	call	watering	
	goto	main
	

watering:
	bank0
	//Encender bypass
	bsf	PORTB_REG, 0x00
	call	sleep_20_seconds
	
	//Encender solenoide 1 y bomba
	bsf	PORTB_REG, 0x02
	bsf	PORTB_REG, 0x01
	call	sleep_60_seconds
	
	//Encender solenoide 2 y apagar la 1
	bsf	PORTB_REG, 0x03
	bcf	PORTB_REG, 0x02
	call	sleep_60_seconds
	
	//Encender solenoide 3 y apagar la 2
	bsf	PORTB_REG, 0x04
	bcf	PORTB_REG, 0x03
	call	sleep_60_seconds
	
	//apago todo
	movlw	0x00
        movwf	PORTB_REG
	
	retlw	0x00
	
	
	
sleep_20_seconds:
	movlw	0x14
	movwf	retardo4
sleep_20_seconds_one:
	decfsz	retardo4
	goto	sleep_20_seconds_two
	retlw	0x00
sleep_20_seconds_two:	
	call	sleep_1_second
	goto	sleep_20_seconds_one
	
	
	
sleep_60_seconds:
	movlw	0x37
	movwf	retardo4
sleep_60_seconds_one:
	decfsz	retardo4
	goto	sleep_60_seconds_two
	retlw	0x00
sleep_60_seconds_two:	
	call	sleep_1_second
	goto	sleep_60_seconds_one
	
	
	
sleep_1_second:
	bank0
	movlw	0x0C	    ;0x2A
	movwf	retardo3

sleep_1_second_three:
	movlw	0xFC
	movwf	retardo2
sleep_1_second_two:
	movlw	0xFA
	movwf	retardo1
sleep_1_second_one:
	decfsz	retardo1
	goto	sleep_1_second_one
	
	decfsz	retardo2
	goto	sleep_1_second_two
	
	decfsz	retardo3
	goto	sleep_1_second_three
	
	retlw	0x00
	
sleep_1_tenth:
	movlw	0x84
	movwf	retardo2
sleep_1_tenth_two:
	movlw	0xFA
	movwf	retardo1
sleep_1_tenth_one:
	decfsz	retardo1
	goto	sleep_1_tenth_one
	
	decfsz	retardo2
	goto	sleep_1_tenth_two
	
	retlw	0x00
END