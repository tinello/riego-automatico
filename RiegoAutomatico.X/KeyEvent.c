/* 
 * File:   KeyEvent.c
 * Author: g
 *
 * Created on 29 de julio de 2024, 12:16
 */

#include "KeyEvent.h"

void key_events(unsigned port_bit_value, regData *keyBits, void (*keyDown)(void), void (*keyUp)(void)) {
    if (port_bit_value == 1 && (*keyBits).bits.bit0 == 0) {
        return;
    }
    
    if (port_bit_value == 0 && (*keyBits).bits.bit0 == 0) {
        (*keyBits).bits.bit0 = 1;      // Flag key down
        return;
    }
    
    if (port_bit_value == 0 && (*keyBits).bits.bit0 == 1 && (*keyBits).bits.bit1 == 0) {
        (*keyBits).bits.bit1 = 1;      // Flag key down function called
        // llamar a funcion key down
        (*keyDown)();
        return;
    }
    
    if (port_bit_value == 1 && (*keyBits).bits.bit1 == 1 && (*keyBits).bits.bit2 == 0) {
        (*keyBits).bits.bit2 = 1;      // Flag key up
        return;
    }
    
    if (port_bit_value == 1 && (*keyBits).bits.bit2 == 1 && (*keyBits).bits.bit3 == 0) {
        (*keyBits).bits.bit3 = 1;      // Flag key up function called
        // llamar a funcion key up
        (*keyUp)();
        (*keyBits).byteData = 0;    // reset all key flags
        return;
    }
    
    if (port_bit_value == 1 && (*keyBits).bits.bit0 == 1) {
        // Aca debe entrar en caso de registrar un key down por ruido
        (*keyBits).byteData = 0;      // reset all key flags
        return;
    }
    
    return;
}



