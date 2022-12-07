;Registros
;--------- BANCO 0 --------
TMR0       equ        001H
PCL        equ        002H
STATUS     equ        003H
FSR        equ        004H
PORTA      equ        005H
PORTB      equ        006H
PCLATH     equ        00AH
INTCON     equ        00BH
PIR1       equ        00CH
PIR2       equ        00DH
TMR1L      equ        00EH
TMR1H      equ        00FH
T1CON      equ        010H
TMR2       equ        011H
T2CON      equ        012H
SSPBUF     equ        013H
SSPCON     equ        014H
CCPR1L     equ        015H
CCPR1H     equ        016H
CCP1CON    equ        017H
ADRESH     equ        01EH
ADCON0     equ        01FH

;--------- BANCO 1 --------
OPTIONREG     equ        081H
TRISA      equ        085H
TRISB      equ        086H
PIE1       equ        08CH
PIE2       equ        08DH
PCON       equ        08EH
OSCCON     equ        08FH
OSCTUNE    equ        090H
PR2        equ        092H
SSPADD     equ        093H
SSPSTAT    equ        094H
ADRESL     equ        09EH
ADCON1     equ        09FH

;--------- BANCO 2 --------
EEDATA     equ        10CH
EEADR      equ        10DH
EEDATH     equ        10EH
EEADRH     equ        10FH

;--------- BANCO 3 --------
EECON1     equ        18CH
EECON2     equ        18DH


;STATUS Bits
IRP        equ        007H
RP1        equ        006H
RP0        equ        005H
TO         equ        004H
PD         equ        003H
Z          equ        002H
DC         equ        001H
C          equ        000H

;INTCON Bits
GIE        equ        007H
PEIE       equ        006H
TMR0IE     equ        005H
INTE       equ        004H
RBIE       equ        003H
TMR0IF     equ        002H
INTF       equ        001H
RBIF       equ        000H

;PIR Bits
ADIF       equ        006H
SSPIF      equ        003H
CCP1IF     equ        002H
TMR2IF     equ        001H
TMR1IF     equ        000H

;PIR2 Bits
EEIF       equ        004H

;T1CON Bits
T1CKPS1    equ        005H
T1CKPS0    equ        004H
T1OSCEN    equ        003H
T1SYNC     equ        002H
TMR1CS     equ        001H
TMR1ON     equ        000H

;T2CON Bits
TOUTPS3    equ        006H
TOUTPS2    equ        005H
TOUTPS1    equ        004H
TOUTPS0    equ        003H
TMR2ON     equ        002H
T2CKPS1    equ        001H
T2CKPS0    equ        001H

;SSPCON Bits
WCOL       equ        007H
SSPOV      equ        006H
SSPEN      equ        005H
CKP        equ        004H
SSPM3      equ        003H
SSPM2      equ        002H
SSPM1      equ        001H
SSPM0      equ        000H

;CCP1CON Bits
CCP1X      equ        005H
CCP1Y      equ        004H
CCP1M3     equ        003H
CCP1M2     equ        002H
CCP1M1     equ        001H
CCP1M0     equ        000H

;ADCON0 Bits
ADCS1      equ        007H
ADCS0      equ        006H
CHS2       equ        005H
CHS1       equ        004H
CHS0       equ        003H
GODONE     equ        002H
ADON       equ        000H

;OPTION Bits
RBPU       equ        007H
INTEDG     equ        006H
T0CS       equ        005H
T0SE       equ        004H
PSA        equ        003H
PS2        equ        002H
PS1        equ        001H
PS0        equ        000H

;PIE1 Bits
ADIE       equ        006H
SSPIE      equ        003H
CCP1IE     equ        002H
TMR2IE     equ        001H
TMR1IE     equ        000H

;PIE2 Bits
EEIE       equ        004H

;PCON Bits
POR        equ        001H
BOR        equ        000H

;OSCCON Bits
IRCF2      equ        006H
IRCF1      equ        005H
IRCF0      equ        004H
IOFS       equ        002H

;OSCTUNE Bits
TUN5       equ        005H
TUN4       equ        004H
TUN3       equ        003H
TUN2       equ        002H
TUN1       equ        001H
TUN0       equ        000H

;SSPSTAT Bits
SMP        equ        007H
CKE        equ        006H
D_A         equ        005H
P          equ        004H
S          equ        003H
RW         equ        002H
UA         equ        001H
BF         equ        000H

;ADCON1 Bits
ADFM       equ        007H
ADCS2      equ        006H
PCFG3      equ        003H
PCFG2      equ        002H
PCFG1      equ        001H
PCFG0      equ        000H

;EECON1 Bits
EEPGD      equ        007H
FREE       equ        004H
WRERR      equ        003H
WREN       equ        002H
WR         equ        001H
RD         equ        000H


W          equ        000H
F          equ        001H

