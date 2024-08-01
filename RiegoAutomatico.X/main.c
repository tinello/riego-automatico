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

#define FLAGS FLAGS
extern volatile unsigned char FLAGS __at(0x21);
typedef union {
    struct {
        unsigned EnableStart            :1;
        unsigned a                   :1;
        unsigned b                 :1;
        unsigned c                   :1;
        unsigned d                   :1;
        unsigned e                   :1;
        unsigned f                   :1;
        unsigned g                    :1;
    };
} FLAGSBits_t;
extern volatile FLAGSBits_t FLAGSBits __at(0x21);


extern regData keyStart __at(0x020);
//extern BitFlags bitFlags __at(0x21);


void configuration(void);
void configuration_oscillator(void);
void configuration_ports(void);
void configuration_timers(void);


void keyStartDown(void);
void keyStartUp(void);


void __interrupt() tcInt(void) {
    
    // Valido que la interrucion sea por TMR0
    if (INTCONbits.TMR0IF == 1) {
        if (FLAGSBits.EnableStart == 0) {
            key_events(PORTAbits.RA0, &keyStart, &keyStartDown, &keyStartUp);
        }
        
        TMR0 = 0;
        INTCONbits.TMR0IF = 0;
    }
}



void main(void) {
    
    configuration();
    
    PORTB = 0x00;
  
    while(1) {
    }
    return;
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
    TRISBbits.TRISB0 = 0; //Output
    TRISBbits.TRISB1 = 0; //Output
    TRISBbits.TRISB2 = 0; //Output
    TRISBbits.TRISB3 = 0; //Output
    TRISBbits.TRISB4 = 0; //Output
    TRISBbits.TRISB5 = 0; //Output
    TRISBbits.TRISB6 = 0; //Output
    TRISBbits.TRISB7 = 0; //Output
    
    ADCON0 = 0x06;
    ADCON1 = 0x06;
    TRISAbits.TRISA0 = 1; //Input
    TRISAbits.TRISA1 = 1; //Input
    TRISAbits.TRISA2 = 1; //Input
    TRISAbits.TRISA3 = 1; //Input
    TRISAbits.TRISA4 = 1; //Input
    TRISAbits.TRISA5 = 1; //Input
    TRISAbits.TRISA6 = 1; //Input
    TRISAbits.TRISA7 = 1; //Input
}

void configuration_timers(void){
    // Configuro Timer0 Prescaler 1:256
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;
    
    OPTION_REGbits.PSA = 0; //Prescaler is assigned to the Timer0 module
    
    // TMR0 Clock Source Select: Internal instruction cycl
    OPTION_REGbits.T0CS = 0;
    
    INTCONbits.GIE = 1; // Enable global interrupt
    INTCONbits.TMR0IE = 1; // Enable TMR0 Overflow Interrupt
    INTCONbits.TMR0IF = 0; // Clear flag TMR0 Overflow Interrupt
    
    TMR0 = 0; // Clear counter
}



void keyStartDown(void){
    
}
void keyStartUp(void){
    
}
