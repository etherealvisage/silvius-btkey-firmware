#include <stdlib.h>
#include <stdint.h>

#include "p32mx250f128b.h"

void setup_sysclk() {
    asm volatile("di");
    // unlock OSCCON
    SYSKEY = 0x0; // write invalid key to force lock
    SYSKEY = 0xAA996655; // Write Key1 to SYSKEY
    SYSKEY = 0x556699AA; // Write Key2 to SYSKEY

    OSCCONbits.NOSC = 1; // internal RC with PLL
    OSCCONbits.PLLMULT = 7; // PLL multiplier of 24x (for 96MHz PLL clock)
    OSCCONbits.PLLODIV = 1; // PLL output divider of 2x (for 48MHz sysclock)
    OSCCONbits.PBDIV = 1; // PB divider of 2x (for 24MHz peripheral clock)

    OSCCONbits.OSWEN = 1; // write to OSWEN to begin switch

    while(OSCCONbits.OSWEN) ;

    SYSKEY = 0x0; // write invalid key to force lock

    asm volatile("ei");
}

void entry() {
    setup_sysclk();

    // start by marking all three as digital pins
    ANSELACLR = 1;
    ANSELBCLR = (1<<13) | (1<<15);
    // mark U1RX (RB13) as input
    TRISBSET = (1<<13);
    // mark U1TX (RB15) as output
    TRISBCLR = (1<<15);
    // Set up PPS for U1
    U1RXR = 0b0011; // RPB13 to U1RX
    RPB15R = 0b0001; // RPB15 to U1TX
    // Initialize U1
    // Set baud rate to 38400 baud
    U1BRG = 38;
    // Set U1MODE to enable (15), (simplex mode 11)
    U1MODE = (1<<11);
    U1MODE |= (1<<15);

    // Enable transmission (10) and receiving (12)
    U1STA = (1<<12) | (1<<10);

    TRISACLR = 1;

    U1TXREG = 'F';
    U1TXREG = 'R';
    U1TXREG = 'M';
    U1TXREG = '!';

    while(1) {}
}

void nmi_handler() {
    
}
