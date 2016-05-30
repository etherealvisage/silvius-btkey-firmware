#include <stdint.h>

#include "button.h"
#include "p32mx250f128b.h"

typedef enum led_state {
    B_INIT_STATE,
    B_POLL_STATE
} button_state;

struct {
    int pressed;
} b_data;

bool button_state_machine() {
    static button_state STATE = B_INIT_STATE;
    switch(STATE) {
    case B_INIT_STATE: {
        // mark pins as digital
        ANSELBCLR = (1<<14) | (1<<15);
        // don't use peripherals
        RPB14R = 0b0000;
        RPB15R = 0b0000;
        // mark pins as in/out
        TRISBSET = (1<<15); // RB15 input
        CNPDBSET = (1<<15); // use pull-down
        TRISBCLR = (1<<14); // RB14 output

        // initial states
        LATBCLR = (1<<15);
        LATBSET = (1<<14);

        // switch states
        STATE = B_POLL_STATE;
        
        break;
    }
    case B_POLL_STATE: {
        uint32_t b = PORTB;
        if(b & (1<<15)) {
            b_data.pressed = 1;
        }
        else {
            b_data.pressed = 0;
        }
        break;
    }
    default:
        break;
    }
    return false;
}

bool button_is_pressed() {
    return b_data.pressed == 1;
}
