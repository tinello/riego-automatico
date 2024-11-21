/*
 * File:   main.c
 * Author: g
 *
 * Created on 5 de agosto de 2024, 23:09
 */


// PIC12F675 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-Up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//Indico al compilador la frecuencia
#define _XTAL_FREQ 4000000


#include <xc.h>
#include "KeyEvent.h"


extern regData keyStart __at(0x050);

extern volatile unsigned char delay1 __at(0x52);
extern volatile unsigned char delay2 __at(0x53);
extern volatile unsigned char delay3 __at(0x54);
extern volatile unsigned int irrigation __at(0x55);


void configuration(void);
void configuration_oscillator(void);
void configuration_ports(void);
void configuration_timers(void);
void configuration_timer0(void);
void configuration_timer1(void);

void keyStartDown(void);
void keyStartUp(void);


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

const unsigned char irWaiting       = 0x00;
const unsigned char irStart         = 0x01;
const unsigned char irDelay1End     = 0x02;
const unsigned char irOnSolenoide   = 0x03;
const unsigned char irDelay2End     = 0x04;
const unsigned char irOnPump        = 0x05;
const unsigned char irIrrigation    = 0x06;
const unsigned char irOffPump       = 0x07;
const unsigned char irDelay3End     = 0x08;
const unsigned char irOffSolenoide  = 0x09;


// TMR1 = 65536-((0,1×4000000)÷(4×8))
// TMR1 = 53036;
// TMR1 = 0xCF2C;
// TMR1H = 0xCF;
// TMR1L = 0x2C;
const unsigned char TMR1H_  = 0xCF;
const unsigned char TMR1L_  = 0x2C;

const unsigned char delay1_value  = 0x64;
const unsigned char delay2_value  = 0x64;
const unsigned char delay3_value  = 0x64;
const unsigned int  irrigation_value  = 0x6464;


void __interrupt() tcInt(void) {
    // Valido que la interrucion sea por TMR0
    if (INTCONbits.TMR0IF == 1) {
        
        if (irrigationSequence == irWaiting) { // Entr solo si no esta iniciada la secuencia de riego.
            key_events(GPIObits.GP3, &keyStart, &keyStartDown, &keyStartUp);
        }
        
        TMR0 = 0;
        INTCONbits.TMR0IF = 0;
    }
    
    
    if (PIR1bits.TMR1IF == 1) {
        PIR1bits.TMR1IF = 0;
        TMR1H = TMR1H_; // Clear counter
        TMR1L = TMR1L_; // Clear counter
        
        if(irrigationSequence == irStart && delay1 > 0){
            delay1--;
            if(delay1 == 0){
                irrigationSequence = irOnSolenoide;
            }
        } else if(irrigationSequence == irOnSolenoide){
            GPIObits.GP0 = 1;
            delay2 = delay2_value;
            irrigationSequence = irDelay2End;
        } else if(delay2 > 0){
            delay2--;
            if(delay2 == 0){
                irrigationSequence = irOnPump;
            }
        } else if(irrigationSequence == irOnPump){
            GPIObits.GP1 = 1;
            //irrigation = 0x1770;
            irrigation = irrigation_value;
            irrigationSequence = irIrrigation;
        } else if(irrigation > 0){
            irrigation--;
            if(irrigation == 0){
                irrigationSequence = irOffPump;
            }
        } else if(irrigationSequence == irOffPump){
            GPIObits.GP1 = 0;
            irrigationSequence = irDelay3End;
            delay3 = delay3_value;
        } else if(delay3 > 0){
            delay3--;
            if(delay3 == 0){
                irrigationSequence = irOffSolenoide;
            }
        } else if(irrigationSequence == irOffSolenoide){
            GPIObits.GP0 = 0;
            irrigationSequence = irWaiting;
            delay1 = 0x00;
            delay2 = 0x00;
            delay3 = 0x00;
        }
    }
}


void main(void) {
    
    configuration();
        
    irrigationSequence = 0x00;
    delay1 = 0x00;
    delay2 = 0x00;
    delay3 = 0x00;
    irrigation = 0x0000;
    
    while(1){
        
    }
    return;
}


void configuration(void) {
    configuration_oscillator();
    configuration_ports();
    configuration_timers();
}

void configuration_oscillator(void) {
    //Internal Oscillator Frequency Select 4MHz
    OSCCAL = 0xFF;
}

void configuration_ports(void){
    GPIO = 0x00;
    CMCON = 0x07;
    ANSEL = 0x00;
    TRISIO = 0x38;
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
    T1CONbits.nT1SYNC = 0;
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
    delay1 = delay1_value;
}
