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
STATUS_REG		EQU 0x03
ADCON0_REG		EQU 0x1F
PORTA_REG		EQU 0x05
PORTB_REG		EQU 0x06
	
;Registros banco 1
OSCCON_REG		EQU 0x8F
TRISA_REG		EQU 0x85
TRISB_REG		EQU 0x86
ADCON1_REG		EQU 0x9F

;STATUS Bits
status_IRP		EQU 0x07
status_RP1		EQU 0x06
status_RP0		EQU 0x05

;PORTA Bits
PORTA_RA0		EQU 0x00
PORTA_RA1_START		EQU 0x01
PORTA_RA2		EQU 0x02
PORTA_RA3		EQU 0x03
PORTA_RA4		EQU 0x04
PORTA_RA5		EQU 0x05
PORTA_RA6		EQU 0x06
PORTA_RA7		EQU 0x07

;PORTB Bits
PORTB_RB0_SOLENOIDE	EQU 0x00
PORTB_RB1_PUMP		EQU 0x01
PORTB_RB2		EQU 0x02
PORTB_RB3		EQU 0x03
PORTB_RB4		EQU 0x04
PORTB_RB5		EQU 0x05
PORTB_RB6		EQU 0x06
PORTB_RB7		EQU 0x07


;Variables
retardo1		EQU 0x20
retardo2		EQU 0x21
retardo3		EQU 0x22
retardo4		EQU 0x23
watering_loops		EQU 0x24
sleep_minuts		EQU 0x25
sleep_seconds		EQU 0x26
watering_count		EQU 0x27
		
	

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
	// Pulsador precionado?
    btfsc	PORTA_REG, PORTA_RA1_START
    goto	main
	// Pulsador precionado, llamo a demora.
    call	sleep_1_tenth
	// Confirmo pulsador precionado?
    btfsc	PORTA_REG, PORTA_RA1_START
    goto	main
	
	
    movlw	0x04
    movwf	watering_count
watering_and_pump_refresh_loop:
    decfsz	watering_count
    goto	watering_and_pump_refresh	
    goto	main
	
	

watering_and_pump_refresh:
	// Llamo a rutina de riego 1
    call	watering
	// 5 Minutos de descanso (porque salta el guarda motor)
    call	pump_refresh
    goto	watering_and_pump_refresh_loop
	


pump_refresh:
	//Sleep 5min
    movlw	0x05
    call	sleep_n_minuts
    retlw	0x00
    
    

watering:
	bank0
	//Encender Solenoide
    bsf	PORTB_REG, PORTB_RB0_SOLENOIDE
	
	//Sleep 5seg
    movlw	0x05
    call	sleep_n_seconds
	
	//Encender bomba
    bsf	PORTB_REG, PORTB_RB1_PUMP
	
	//Loop for 5 times 1 minute sleep
    movlw	0x05
    movwf	watering_loops

watering_loop:
    call	sleep_60_seconds		
    decfsz	watering_loops
    goto	watering_loop
	
pump_power_off:
	//Apagar bomba
    bcf	PORTB_REG, PORTB_RB1_PUMP
	
	//Sleep 4seg
    call	sleep_1_second
    call	sleep_1_second
    call	sleep_1_second
    call	sleep_1_second
	
	//Apagar Solenoide
    bcf	PORTB_REG, PORTB_RB0_SOLENOIDE
	
    retlw	0x00
	
	

// La cantidad de segundos se debe enviar en el registro W. W > 0
sleep_n_seconds:
    movwf	sleep_seconds
sleep_n_seconds_one:
    call	sleep_1_second
    decfsz	sleep_seconds
    goto	sleep_n_seconds_one
    retlw	0x00


// La cantidad de minutos se debe enviar en el registro W. W > 0
sleep_n_minuts:
    movwf	sleep_minuts
sleep_n_minuts_one:
    call	sleep_60_seconds
    decfsz	sleep_minuts
    goto	sleep_n_minuts_one
    retlw	0x00
    
    
	
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