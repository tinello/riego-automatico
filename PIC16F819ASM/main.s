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
	goto	main

	
PSECT	code, delta=2, abs
ORG	0x000A
main:
conf_internal_clock_8mhz:
	bank1
	movlw	0x64
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
	
	
loop:
	btfsc	PORTA_REG, 0x01
	goto	loop
	call	retardo_1_decima
	btfsc	PORTA_REG, 0x01
	goto	loop
	call	regar	
	goto	loop
	

regar:
	bank0
	//Encender bypass
	bsf	PORTB_REG, 0x00
	call	retardo
	
	//Encender solenoide 1 y bomba
	bsf	PORTB_REG, 0x02
	bsf	PORTB_REG, 0x01
	call	retardo
	
	//Encender solenoide 2 y apagar la 1
	bsf	PORTB_REG, 0x03
	bcf	PORTB_REG, 0x02
	call	retardo
	
	//Encender solenoide 3 y apagar la 2
	bsf	PORTB_REG, 0x04
	bcf	PORTB_REG, 0x03
	call	retardo
	
	//apago todo
	movlw	0x00
        movwf	PORTB_REG
	
	retlw	0x00
	
	
retardo:
	bank0
	movlw	0x2A
	movwf	retardo3

tres:
	movlw	0xFC
	movwf	retardo2
dos:
	movlw	0xFA
	movwf	retardo1
uno:
	decfsz	retardo1
	goto	uno
	
	decfsz	retardo2
	goto	dos
	
	decfsz	retardo3
	goto	tres
	
	retlw	0x00
	
retardo_1_decima:
	movlw	0x84
	movwf	retardo2
retardo_1_decima_dos:
	movlw	0xFA
	movwf	retardo1
retardo_1_decima_uno:
	decfsz	retardo1
	goto	retardo_1_decima_uno
	
	decfsz	retardo2
	goto	retardo_1_decima_dos
	
	retlw	0x00
END