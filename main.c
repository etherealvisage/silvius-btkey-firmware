#include <stdlib.h>
#include <stdint.h>

#include "p32mx250f128b.h"

#include "bluetooth.h"
#include "keyboard.h"
#include "button.h"

typedef bool (*state_machine_ptr)();

const state_machine_ptr state_machines[] = {
    //bluetooth_state_machine,
    keyboard_state_machine,
    button_state_machine,
};

void setup_sysclk() {
    asm volatile("di");
    // unlock OSCCON
    SYSKEY = 0x0; // write invalid key to force lock
    SYSKEY = 0xAA996655; // Write Key1 to SYSKEY
    SYSKEY = 0x556699AA; // Write Key2 to SYSKEY

    OSCCONbits.NOSC = 2; // external crystal (24MHz)
    OSCCONbits.PBDIV = 1; // PB divider of 2x (for 12MHz peripheral clock)

    OSCCONbits.OSWEN = 1; // write to OSWEN to begin switch

    while(OSCCONbits.OSWEN) ;

    SYSKEY = 0x0; // write invalid key to force lock

    asm volatile("ei");
}

void format32(char *p, uint32_t v) {
    const char *hex = "0123456789abcdef";
    int i;
    for(i = 0; i < 8; i ++) {
        p[i] = hex[v >> 28];
        v <<= 4;
    }
}

void entry() {
    // LED setup
    {
        // mark as digital
        ANSELACLR = 1;
        // set as output
        TRISACLR = 1;
    }
    LATACLR = 1;
    setup_sysclk();
    LATASET = 1;

    #if 0
    // U1 setup
    {
        // mark pins as digital
        ANSELBCLR = (1<<13) | (1<<15);
        // mark U1RX (RB13) as input
        TRISBSET = (1<<13);
        // mark U1TX (RB15) as output
        TRISBCLR = (1<<15);
        // Set up PPS for U1
        U2RXR = 0b0000; // RPA1 to U2RX
        RPB0R = 0b0010; // RPB0 to U2TX
        // Initialize U1
        // Set baud rate to 19200 baud
        U1BRG = 38;
        // Enable U1
        U1MODEbits.ON = 1;
        // Use simplex mode (two-wire)
        U1MODEbits.RTSMD = 1;
        // Enable transmission and receiving
        U1STAbits.UTXEN = 1;
        U1STAbits.URXEN = 1;
    }
    #endif

    // U2 setup
    {
        // mark pins as digital
        ANSELACLR = 1<<1;
        ANSELBCLR = 1<<0;
        // mark U2RX (RA1) as input
        TRISASET = (1<<1);
        // mark U2TX (RB0) as output
        TRISBCLR = (1<<0);
        // set up PPS for U2
        U2RXR = 0b0000;
        RPB0R = 0b0010;
        // Initialize U2
        // Set baud rate to 19200 baud
        U2BRG = 38;
        // Set U2MODE to enable, simplex
        U2MODEbits.RTSMD = 1; // simplex
        U2MODEbits.ON = 1;
        // Enable transmission and receiving
        U2STAbits.URXEN = 1;
        U2STAbits.UTXEN = 1;
    }

    // Let any listeners know we've finished initializing
    //U1TXREG = '~';
    U2TXREG = '~';
    
    LATACLR = 1;

    int j = 0;
    while(1) {
        int i = 0;
        bool idle = true;

        for(; i < sizeof(state_machines)/sizeof(state_machine_ptr); i ++) {
            idle = state_machines[i]() && idle;
        }

        if(idle) {
            /* TODO: enter power-saving mode here */
        }
        if(j <= 0) {
            //LATAINV = 1;
            j = 100000;
        }
        j --;
    }
}

void nmi_handler() {
    
}
