#include <stdint.h>

#include "bluetooth.h"
#include "p32mx250f128b.h"

typedef enum bluetooth_state {
    BT_INIT_STATE,
    BT_LISTEN_STATE,
    BT_READ_STATE,
    BT_PRESS_STATE,
    BT_RELEASE_STATE
} bluetooth_state;

struct {
    bluetooth_state next_state;
    char buffer[32];
    int buffer_offset, buffer_length;
} bt_data;

bool bluetooth_state_machine() {
    static bluetooth_state STATE = BT_INIT_STATE;

    bool idle = false;

    switch(STATE) {
    case BT_INIT_STATE: {
        // mark pins as digital
        ANSELBCLR = (1<<13) | (1<<15);
        // Set up PPS for U1
        U1RXR = 0b0011; // RPB13 to U1RX
        RPB15R = 0b0001; // RPB15 to U1TX
        // Initialize U1
        // Set baud rate to 9600 baud
        U1BRG = 152;
        // Enable U1
        U1MODEbits.ON = 1;
        // Use simplex mode (two-wire)
        U1MODEbits.RTSMD = 1;

        // Enable transmission and receiving
        U1STAbits.UTXEN = 1;
        U1STAbits.URXEN = 1;
        
        STATE = BT_LISTEN_STATE;

        break;
    }
    case BT_LISTEN_STATE: {
        /* if there's no data waiting, we're done. */
        if(U1STAbits.URXDA == 0) {
            idle = true;
            break;
        }

        /* grab the command */
        uint8_t cmd = U1RXR;

        if(cmd == 'p') {
            STATE = BT_READ_STATE;
            bt_data.buffer_offset = 0;
            bt_data.buffer_length = 4;
            bt_data.next_state = BT_PRESS_STATE;
        }
        else if(cmd == 'r') {
            STATE = BT_READ_STATE;
            bt_data.buffer_offset = 0;
            bt_data.buffer_length = 4;
            bt_data.next_state = BT_RELEASE_STATE;
        }
        else {
            STATE = BT_LISTEN_STATE;
        }

        break;
    }
    case BT_PRESS_STATE: {
        /* TODO: handle keypress event */
        STATE = BT_LISTEN_STATE;
        break;
    }
    case BT_RELEASE_STATE: {
        /* TODO: handle keypress event */
        STATE = BT_LISTEN_STATE;
        break;
    }
    default:
        /* Should never be reached. */
        idle = true;
        break;
    }

    return idle;
}
