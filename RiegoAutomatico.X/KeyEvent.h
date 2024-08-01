/* 
 * File:   KeyEvent.h
 * Author: g
 *
 * Created on 29 de julio de 2024, 12:16
 */


struct bitStructure{
    unsigned bit0:1;
    unsigned bit1:1;
    unsigned bit2:1;
    unsigned bit3:1;
    unsigned bit4:1;
    unsigned bit5:1;
    unsigned bit6:1;
    unsigned bit7:1;
};

typedef union {
    unsigned byteData;
    struct bitStructure bits;
} regData;

//extern regData regData2;

/**
 * Este metodo recibe el estado de un bit de un puerto, un puntero a un registro
 * y 2 punteros a funciones.
 * En base al estado del bit del puerto, llama a las funciones keyDoun y keyUp 
 * al pulsar y soltar.
 * Trabaja con un estado normal 1 y cuando cambia a 0 comienza las validaciones.
 * @param key_state El estado del pulsador. Debe ser normal alto (1)
 * @param regKey Puntero al registro que lleva el control del estado. Se debe 
 * enviar con cada llamado del mismo bit.
 * @param keyDown Puntero a la funcion que se llamara cunado sea un cambio de 
 * estado valido de 1 a 0
 * @param keyUp Puntero a la funcion que se llamara cunado sea un cambio de 
 * pulso valido de 0 a 1
 */
void key_events(unsigned key_state, regData *regKey, void (*keyDown)(void), void (*keyUp)(void));

