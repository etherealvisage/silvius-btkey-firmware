#include <stdlib.h>
#include <stdint.h>

#include "p32mx250f128b.h"

#include "bluetooth.h"

typedef bool (*state_machine_ptr)();

state_machine_ptr state_machines[] = {
    bluetooth_state_machine,
};

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
    U2RXR = 0b0000; // RPA1 to U2RX
    RPB0R = 0b0010; // RPB0 to U2TX
    // Initialize U1
    // Set baud rate to 38400 baud
    U1BRG = 38;
    // Enable U1
    U1MODEbits.ON = 1;
    // Use simplex mode (two-wire)
    U1MODEbits.RTSMD = 1;

    // Enable transmission and receiving
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;

    // set RA0 (LED) to output
    TRISACLR = 1;

    // Let any listeners know we've finished initializing
    U1TXREG = '~';

    while(1) {
        int i;
        bool idle = true;
        for(i = 0; i < sizeof(state_machines)/sizeof(state_machine_ptr); i ++) {
            idle = state_machines[i]() && idle;
        }

        if(idle) {
            /* TODO: enter power-saving mode here */
        }
    }
}

void nmi_handler() {
    
}
