 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include "KeyEvent.h"

//Indico al compilador la frecuencia
#define _XTAL_FREQ 16000000

void configuration_timer1(void);

void keyStartDown(void);
void keyStartUp(void);

void irrigation_sequence(void);


extern regData keyStart __at(0x020);

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

const unsigned char TMR1H_  = 0x3C;
const unsigned char TMR1L_  = 0xB2;

const unsigned char delayBetweenStartToSolenoide_value  = 0x09;
const unsigned char delayBetweenSolenoideToPump_value  = 0x05;
const unsigned char delayBetweenPumpToSolenoide_value  = 0x09;
const unsigned int  irrigation_value  = 0x012C; // 0x012C => 300
const unsigned char delayOneSecond_value = 0x09;
const unsigned int  delayPumpRefresh_value = 0x012C; // 0x012C => 300


void __interrupt() tcInt(void) {
    // Valido que la interrucion sea por TMR1 / timer de riego
    if (PIR1bits.TMR1IF == 1) {
        TMR1H = TMR1H_; // Clear counter
        TMR1L = TMR1L_; // Clear counter
        PIR1bits.TMR1IF = 0;
        
        if (irrigationSequence == irWaiting) { // Entra solo si no esta iniciada la secuencia de riego.
            key_events(PORTAbits.RA0, &keyStart, &keyStartDown, &keyStartUp);
        }
        
        if (delayOneSecond-- == 0){
            delayOneSecond = delayOneSecond_value;
            FLAGSbits.TIMER_1_S = 1;
        }
    }
}

/*
    Main application
*/

int main(void)
{
    SYSTEM_Initialize();
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to: 

    configuration_timer1();
    
    
    // Enable the Global Interrupts 
    INTERRUPT_GlobalInterruptEnable(); 

    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
    INTERRUPT_PeripheralInterruptEnable(); 

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable(); 


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


void configuration_timer1(void){
    // Config Timer1 prescala 1:8
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1OSCEN = 1; // Oscillator is enabled
    T1CONbits.TMR1CS0 = 0; // Internal clock, clock source is instruction clock (FOSC/4)
    T1CONbits.TMR1CS1 = 0; // Internal clock, clock source is instruction clock (FOSC/4)
    T1CONbits.TMR1ON = 0; // Stops Timer1
    
    //INTCONbits.PEIE = 1; // Enables all unmasked peripheral interrupts
    
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