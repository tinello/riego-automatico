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
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF         // Low-Voltage Programming Enable bit (RB3/PGM pin has PGM function, Low-Voltage Programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB2      // CCP1 Pin Selection bit (CCP1 function on RB2)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

//Indico al compilador la frecuencia
#define _XTAL_FREQ 8000000

void main(void){
    
    //Internal Oscillator Frequency Select 8MHz
    //OSCCON = 0x76;
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;

    //Frequency is stable
    OSCCONbits.IOFS = 1;
    
    
    //TRISB = 0; //RB0 as Output PIN
    TRISBbits.TRISB0 = 0; //Output solenoide 1
    TRISBbits.TRISB1 = 0; //Output solenoide 2
    TRISBbits.TRISB2 = 0; //Output solenoide 3
    TRISBbits.TRISB3 = 0; //Output
    TRISBbits.TRISB4 = 0; //Output bypass tanque/riego
    TRISBbits.TRISB5 = 0; //Output bomba de agua
    TRISBbits.TRISB6 = 0; //Output
    TRISBbits.TRISB7 = 0; //Output

    PORTB = 0x00;
  
  
    while(1) {
        PORTBbits.RB4 = 1;  // Activo bypass riego
        __delay_ms(1000); // Delay bypass finish
        
        PORTBbits.RB0 = 1;  // Solenoide 1 ON
        PORTBbits.RB5 = 1;  // Bomba ON
        __delay_ms(5000); // Riego
        PORTBbits.RB5 = 0;  // Bomba OFF
        PORTBbits.RB0 = 0;  // Solenoide 1 OFF
        
        
        __delay_ms(5000); // Espera aque se llene el cisterna
        
        
        PORTBbits.RB1 = 1;  // Solenoide 2 ON
        PORTBbits.RB5 = 1;  // Bomba ON
        __delay_ms(5000); // Riego
        PORTBbits.RB5 = 0;  // Bomba OFF
        PORTBbits.RB1 = 0;  // Solenoide 2 OFF
        
        
        __delay_ms(5000); // Espera aque se llene el cisterna
        
        
        PORTBbits.RB2 = 1;  // Solenoide 3 ON
        PORTBbits.RB5 = 1;  // Bomba ON
        __delay_ms(5000); // Riego
        PORTBbits.RB5 = 0;  // Bomba OFF
        PORTBbits.RB2 = 0;  // Solenoide 3 OFF
        
        PORTBbits.RB4 = 0;  // Desactivo bypass riego
        
        
        __delay_ms(10000); // Espera aque se llene el cisterna
    }
    return;
}
