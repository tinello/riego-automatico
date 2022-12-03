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

//Indico al compilador la frecuencia
#define _XTAL_FREQ 8000000

void configuration(void);
void bypassToGarden(void);
void bypassToTank(void);
void irrigationDelay(void);
void loadTankDelay(void);
void irrigationZone1(void);
void irrigationZone2(void);
void irrigationZone3(void);
void setSolenoide1(unsigned char);
void setSolenoide2(unsigned char);
void setSolenoide3(unsigned char);
void setOnSolenoide1(void);
void setOffSolenoide1(void);
void setOnSolenoide2(void);
void setOffSolenoide2(void);
void setOnSolenoide3(void);
void setOffSolenoide3(void);
void setBypass(unsigned char);
void setPump(unsigned char);
void setPumpOn(void);
void setPumpOff(void);


void main(void) {
    
    configuration();
    
    PORTB = 0x00;
  
    while(1) {        
        if (PORTAbits.RA1 == 0) {
            __delay_ms(100);            
            if (PORTAbits.RA1 == 0) {
                bypassToGarden();
                irrigationZone1();
                loadTankDelay();
                irrigationZone2();
                loadTankDelay();
                irrigationZone3();
                bypassToTank();
            }
        }
    }
    return;
}

void configuration(void) {
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
    
    ADCON0 = 0x06;
    ADCON1 = 0x06;
    TRISAbits.TRISA0 = 1; //Input Start Irrigation
    TRISAbits.TRISA1 = 1; //Input
    TRISAbits.TRISA2 = 1; //Input
    TRISAbits.TRISA3 = 1; //Input
    TRISAbits.TRISA4 = 1; //Input
    TRISAbits.TRISA5 = 1; //Input
    TRISAbits.TRISA6 = 1; //Input
    TRISAbits.TRISA7 = 1; //Input
}

void bypassToGarden(void) {
    setBypass(1);  // Activo bypass riego
    __delay_ms(1000); // Delay bypass finish
}

void bypassToTank(void) {
    setBypass(0);  // Desactivo bypass riego
}

void irrigationDelay(void) {
    __delay_ms(5000); // Riego
}

void loadTankDelay(void) {
    __delay_ms(5000); // Espera aque se llene el cisterna
}

void setOnSolenoide1(void) {
    setSolenoide1(1);
}

void setOffSolenoide1(void) {
    setSolenoide1(0);
}

void setSolenoide1(unsigned char state) {
    PORTBbits.RB2 = state;
}

void setOnSolenoide2(void) {
    setSolenoide2(1);
}

void setOffSolenoide2(void) {
    setSolenoide2(0);
}

void setSolenoide2(unsigned char state) {
    PORTBbits.RB3 = state;
}

void setOnSolenoide3(void) {
    setSolenoide3(1);
}

void setOffSolenoide3(void) {
    setSolenoide3(0);
}
void setSolenoide3(unsigned char state) {
    PORTBbits.RB4 = state;
}

void setBypass(unsigned char state) {
    PORTBbits.RB0 = state;
}

void setPump(unsigned char state) {
    PORTBbits.RB1 = state;
}

void setPumpOn(void) {
    setPump(1); // Bomba ON
}

void setPumpOff(void) {
    setPump(0); // Bomba OFF
}

void irrigationZone1(void) {
    setOnSolenoide1();
    setPumpOn();
    irrigationDelay();
    setPumpOff();
    setOffSolenoide1();
}

void irrigationZone2(void) {
    setOnSolenoide2();
    setPumpOn();
    irrigationDelay();
    setPumpOff();
    setOffSolenoide2();
}

void irrigationZone3(void) {
    setOnSolenoide3();
    setPumpOn();
    irrigationDelay();
    setPumpOff();
    setOffSolenoide3();
}
