#include "keyboard.h"

#include "usb/include/usb.h"

typedef enum keyboard_state {
    K_INIT_STATE,
    K_WAIT_STATE,
} bluetooth_state;

struct {
    
} k_data;

bool keyboard_state_machine() {
    static keyboard_state STATE = K_INIT_STATE;
    switch(STATE) {
    case K_INIT_STATE: {
        STATE = K_WAIT_STATE;
        break;
    }
    case K_WAIT_STATE: {
        usb_service();
        break;
    }
    }
    return false;
}
