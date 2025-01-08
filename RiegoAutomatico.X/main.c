/*
 * File:   main.c
 * Author: gustavo
 *
 * Created on 2 de octubre de 2022, 15:48
 */




// PIC16F819 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB2      // CCP1 Pin Selection bit (CCP1 function on RB2)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "KeyEvent.h"

//Indico al compilador la frecuencia
#define _XTAL_FREQ 8000000

extern regData keyStart __at(0x020);

// TMR1 = 65536-((0,1×8000000)÷(4×8))
// TMR1 = 40536;
// TMR1 = 0x9E58;
// TMR1H = 0x9E;
// TMR1L = 0x58;


extern volatile unsigned char delayBetweenStartToSolenoide  __at(0x22);
extern volatile unsigned char delayBetweenSolenoideToPump   __at(0x23);
extern volatile unsigned char delayBetweenPumpToSolenoide   __at(0x24);
extern volatile unsigned int  delayPumpRefresh              __at(0x25);
extern volatile unsigned int  irrigation                    __at(0x27);

extern volatile unsigned char FLAGS                         __at(0x29);
typedef union {
    struct {
        unsigned TIMER_100_MS           :1;
        unsigned TIMER_1_S              :1;
        unsigned RB2                    :1;
        unsigned RB3                    :1;
        unsigned RB4                    :1;
        unsigned RB5                    :1;
        unsigned RB6                    :1;
        unsigned RB7                    :1;
    };
} FLAGSbits_t;
extern volatile FLAGSbits_t FLAGSbits __at(0x2A);

extern volatile unsigned char irrigationCycles   __at(0x2B);



void configuration(void);
void configuration_oscillator(void);
void configuration_ports(void);
void configuration_timers(void);
void configuration_timer0(void);
void configuration_timer1(void);


void keyStartDown(void);
void keyStartUp(void);

void irrigation_sequence(void);

/**
 * 0x00 Waitting
 * 0x01 Start Irrigation
 * 0x02 Delay 1 end
 * 0x03 On Solenoides
 * 0x04 Delay 2 end
 * 0x05 On Pump
 * 0x06 Irrigation
 * 0x07 Off Pump
 * 0x08 Delay 3 end
 * 0x09 Off Solenoide
*/
unsigned char irrigationSequence;
unsigned char delayOneSecond;

const unsigned char irWaiting        = 0x00;
const unsigned char irStart          = 0x01;
const unsigned char irDelay1End      = 0x02;
const unsigned char irOnSolenoide    = 0x03;
const unsigned char irDelay2End      = 0x04;
const unsigned char irOnPump         = 0x05;
const unsigned char irIrrigation     = 0x06;
const unsigned char irOffPump        = 0x07;
const unsigned char irDelay3End      = 0x08;
const unsigned char irOffSolenoide   = 0x09;
const unsigned char irPumpRefresh    = 0x0A;
const unsigned char irOffPumpRefresh = 0x0B;

const unsigned char TMR1H_  = 0x9E;
const unsigned char TMR1L_  = 0x58;

const unsigned char delayBetweenStartToSolenoide_value  = 0x09;
const unsigned char delayBetweenSolenoideToPump_value  = 0x05;
const unsigned char delayBetweenPumpToSolenoide_value  = 0x09;
const unsigned int  irrigation_value  = 0x012C; // 0x012C => 300
const unsigned char delayOneSecond_value = 0x09;
const unsigned int  delayPumpRefresh_value = 0x012C; // 0x012C => 300


void __interrupt() tcInt(void) {
    
    // Valido que la interrucion sea por TMR0 / para validar si el boton de inicio se pulso.
    if (INTCONbits.TMR0IF == 1) {
        if (irrigationSequence == irWaiting) { // Entra solo si no esta iniciada la secuencia de riego.
            key_events(PORTAbits.RA0, &keyStart, &keyStartDown, &keyStartUp);
        }
        
        TMR0 = 0;
        INTCONbits.TMR0IF = 0;
    }
    
    // Valido que la interrucion sea por TMR1 / timer de riego
    if (PIR1bits.TMR1IF == 1) {
        PIR1bits.TMR1IF = 0;
        TMR1H = TMR1H_; // Clear counter
        TMR1L = TMR1L_; // Clear counter
        
        FLAGSbits.TIMER_100_MS = 1;
        
        if (delayOneSecond-- == 0){
            delayOneSecond = delayOneSecond_value;
            FLAGSbits.TIMER_1_S = 1;
        }
    }
}



void main(void) {
    
    configuration();
    
    PORTB = 0x00;
    
    FLAGS = 0x00;
    
    irrigationSequence = 0x00;
    delayBetweenStartToSolenoide = 0x00;
    delayBetweenSolenoideToPump = 0x00;
    delayBetweenPumpToSolenoide = 0x00;
    irrigation = 0x0000;
    delayOneSecond = delayOneSecond_value;
  
    while(1) {
        if(FLAGSbits.TIMER_1_S == 1){
            FLAGSbits.TIMER_1_S = 0;
            if (irrigationSequence != irWaiting) { //evito que entre al ciclo de riego, si esta en espera.
                irrigation_sequence();
            }
        }
    }
    return;
}

void irrigation_sequence(void) {
    if(irrigationSequence == irStart && delayBetweenStartToSolenoide > 0){ // Retardo de inicio
        delayBetweenStartToSolenoide--;
        if(delayBetweenStartToSolenoide == 0){
            irrigationSequence = irOnSolenoide;
        }
    } else if(irrigationSequence == irOnSolenoide){ // Activo solenoide
        PORTBbits.RB0 = 1;
        delayBetweenSolenoideToPump = delayBetweenSolenoideToPump_value;
        irrigationSequence = irDelay2End;
    } else if(delayBetweenSolenoideToPump > 0){                          // Retardo entre solenoide y bomba
        delayBetweenSolenoideToPump--;
        if(delayBetweenSolenoideToPump == 0){
            irrigationSequence = irOnPump;
        }
    } else if(irrigationSequence == irOnPump){      // Activo bomba
        PORTBbits.RB1 = 1;
        //irrigation = 0x1770;
        irrigation = irrigation_value;
        irrigationSequence = irIrrigation;
    } else if(irrigation > 0){                      // Retardo de riego
        irrigation--;
        if(irrigation == 0){
            irrigationSequence = irOffPump;
        }
    } else if(irrigationSequence == irOffPump){     // Apago bomba
        PORTBbits.RB1 = 0;
        irrigationSequence = irDelay3End;
        delayBetweenPumpToSolenoide = delayBetweenPumpToSolenoide_value;
    } else if(delayBetweenPumpToSolenoide > 0){                          // Retardo para apagar solenoides
        delayBetweenPumpToSolenoide--;
        if(delayBetweenPumpToSolenoide == 0){
            irrigationSequence = irOffSolenoide;
        }
    } else if(irrigationSequence == irOffSolenoide){ // Apago solenoide y fin de riego
        PORTBbits.RB0 = 0;
        delayPumpRefresh = delayPumpRefresh_value;
        irrigationSequence = irPumpRefresh;
    } else if(irrigationSequence == irPumpRefresh){ // Retardo de enfriamiento de bomba
        delayPumpRefresh--;
        if(delayPumpRefresh == 0){
            irrigationSequence = irOffPumpRefresh;
        }
    } else if(irrigationSequence == irOffPumpRefresh){ // Retardo de enfriamiento de bomba
        if(irrigationCycles > 0) {
            irrigationCycles--;
            irrigationSequence = irStart;
            delayBetweenStartToSolenoide = delayBetweenStartToSolenoide_value; 
        } else {
            irrigationSequence = irWaiting;
        }
    }
}

void configuration(void) {
    configuration_oscillator();
    configuration_ports();
    configuration_timers();
}

void configuration_oscillator(void) {
    //Internal Oscillator Frequency Select 8MHz
    //OSCCON = 0x76;
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;

    //Frequency is stable
    OSCCONbits.IOFS = 1;
}

void configuration_ports(void){
    //TRISB = 0;
    TRISBbits.TRISB0 = 0; //Output Solenoide
    TRISBbits.TRISB1 = 0; //Output Pump
    TRISBbits.TRISB2 = 0; //Output
    TRISBbits.TRISB3 = 0; //Output
    TRISBbits.TRISB4 = 0; //Output
    TRISBbits.TRISB5 = 0; //Output
    TRISBbits.TRISB6 = 0; //Output
    TRISBbits.TRISB7 = 0; //Output
    
    ADCON0 = 0x06;
    ADCON1 = 0x06;
    TRISAbits.TRISA0 = 1; //Input Button
    TRISAbits.TRISA1 = 1; //Input
    TRISAbits.TRISA2 = 1; //Input
    TRISAbits.TRISA3 = 1; //Input
    TRISAbits.TRISA4 = 1; //Input
    TRISAbits.TRISA5 = 1; //Input
    TRISAbits.TRISA6 = 1; //Input
    TRISAbits.TRISA7 = 1; //Input
}

void configuration_timers(void){
    configuration_timer0();
    configuration_timer1();
    
    INTCONbits.GIE = 1; // Enable global interrupt
}
void configuration_timer0(void){
    // Configuro Timer0 Prescaler 1:256
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;
    
    OPTION_REGbits.PSA = 0; //Prescaler is assigned to the Timer0 module
    
    // TMR0 Clock Source Select: Internal instruction cycl
    OPTION_REGbits.T0CS = 0;
    
    INTCONbits.TMR0IE = 1; // Enable TMR0 Overflow Interrupt
    INTCONbits.TMR0IF = 0; // Clear flag TMR0 Overflow Interrupt
    
    TMR0 = 0; // Clear counter
}

void configuration_timer1(void){
    // Config Timer1 prescala 1:8
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1OSCEN = 1; // Oscillator is enabled
    T1CONbits.T1INSYNC = 0;
    T1CONbits.TMR1CS = 0; // Internal clock
    T1CONbits.TMR1ON = 0; // Stops Timer1
    
    INTCONbits.PEIE = 1; // Enables all unmasked peripheral interrupts
    
    PIR1bits.TMR1IF = 0; // Clear flag
    
    TMR1H = TMR1H_; // Clear counter 0.1seg
    TMR1L = TMR1L_; // Clear counter 0.1seg
    
    PIE1bits.TMR1IE = 1; // TMR1 overflow interrupt enable
    
    T1CONbits.TMR1ON = 1; // Start Timer1
}


void keyStartDown(void){
    
}
void keyStartUp(void){
    irrigationSequence = irStart;
    delayBetweenStartToSolenoide = delayBetweenStartToSolenoide_value;
    irrigationCycles = 0x04;
}
