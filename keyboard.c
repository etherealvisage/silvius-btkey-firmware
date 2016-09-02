#include "keyboard.h"
#include "button.h"

#include "usb/include/usb.h"
#include "usb/include/usb_hid.h"
#include "usb_config.h"

typedef enum keyboard_state {
    K_INIT_STATE,
    K_WAIT_STATE,
} keyboard_state;

struct {
    uint8_t type_buffer[64];
    int type_buffer_start;
    int type_buffer_count;
    int type_pressed;
    int cooldown_since_last;
} k_data;

static void keyboard_convert_ascii(uint8_t *mod, uint8_t *value, uint8_t *skey,
    uint8_t c);

bool keyboard_state_machine() {
    static keyboard_state STATE = K_INIT_STATE;
    switch(STATE) {
    case K_INIT_STATE: {
        usb_init();
        STATE = K_WAIT_STATE;

        k_data.type_buffer_start = 0;
        k_data.type_buffer_count = 0;
        k_data.type_pressed = 0;
        k_data.cooldown_since_last = 0;
        break;
    }
    case K_WAIT_STATE: {
        if(k_data.cooldown_since_last > 0) {
            k_data.cooldown_since_last --;
            break;
        }
        k_data.cooldown_since_last = 1000;
        if(usb_is_configured() &&
            !usb_in_endpoint_halted(1) &&
            !usb_in_endpoint_busy(1)) {

            unsigned char *buf = usb_get_in_buffer(1);
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
            if(k_data.type_buffer_count > 0 || k_data.type_pressed) {
                if(k_data.type_pressed) buf[3] = 0x0, k_data.type_pressed = 0;
                else {
                    keyboard_convert_ascii(buf + 0, buf + 3, buf + 4,
                        k_data.type_buffer[k_data.type_buffer_start]);
                    k_data.type_buffer_count --;
                    k_data.type_buffer_start ++;
                    k_data.type_buffer_start %= 64;
                    k_data.type_pressed = 1;
                }
            }
            else buf[3] = 0x0;
            buf[4] = 0x0;
            buf[5] = 0x0;
            buf[6] = 0x0;
            buf[7] = 0x0;
            usb_send_in_buffer(1, 8);
        }
        usb_service();
        break;
    }
    default:
        break;
    }
    return false;
}

void keyboard_type(char c) {
    int index = (k_data.type_buffer_start + k_data.type_buffer_count) % 64;
    k_data.type_buffer[index] = c;
    k_data.type_buffer_count ++;
}

static const uint8_t ascii_to_usb[256][3] = {
    {0,0,0},
#include "ascii_to_usb.h"
};

static void keyboard_convert_ascii(uint8_t *mod, uint8_t *key, uint8_t *skey,
    uint8_t c) {

    *mod = ascii_to_usb[c][0];
    *key = ascii_to_usb[c][1];
    *skey = ascii_to_usb[c][2];

#if 0
    // Letters
    if(c >= 'a' && c <= 'z') {
        *key = (c - 'a' + 4);
    }
    else if(c >= 'A' && c <= 'Z') {
        *mod |= 2; // left shift key
        *skey = 225;
        *key = (c - 'A' + 4);
    }
    // Numbers
    else if(c == '0') {
        *key = 39;
    }
    else if(c >= '1' && c <= '9') {
        *key = (c - '1' + 30);
    }
    // Whitespaces
    else if(c == ' ') {
        *key = 44;
    }
    else if(c == '\t') {
        *key = 43;
    }
    else if(c == '\r') {
        *key = 40;
    }
    else if(c == '\b') {
        *key = 42;
    }
    else if(c == '\x7f') {
        *key = 76;
    }
    else {
        *key = 0;
    }
#endif
}

/* Callbacks. These function names are set in usb_config.h. */
void app_set_configuration_callback(uint8_t configuration)
{

}

uint16_t app_get_device_status_callback()
{
	return 0x0000;
}

void app_endpoint_halt_callback(uint8_t endpoint, bool halted)
{

}

int8_t app_set_interface_callback(uint8_t interface, uint8_t alt_setting)
{
	return 0;
}

int8_t app_get_interface_callback(uint8_t interface)
{
	return 0;
}

void app_out_transaction_callback(uint8_t endpoint)
{

}

void app_in_transaction_complete_callback(uint8_t endpoint)
{

}

int8_t app_unknown_setup_request_callback(const struct setup_packet *setup)
{
	/* To use the HID device class, have a handler for unknown setup
	 * requests and call process_hid_setup_request() (as shown here),
	 * which will check if the setup request is HID-related, and will
	 * call the HID application callbacks defined in usb_hid.h. For
	 * composite devices containing other device classes, make sure
	 * MULTI_CLASS_DEVICE is defined in usb_config.h and call all
	 * appropriate device class setup request functions here.
	 */
	return process_hid_setup_request(setup);
}

int16_t app_unknown_get_descriptor_callback(const struct setup_packet *pkt, const void **descriptor)
{
	return -1;
}

void app_start_of_frame_callback(void)
{

}

void app_usb_reset_callback(void)
{

}

/* HID Callbacks. See usb_hid.h for documentation. */

static uint8_t report_buf[3];

static void get_report_callback(bool transfer_ok, void *context)
{
	/* Nothing to do here really. It either succeeded or failed. If it
	 * failed, the host will ask for it again. It's nice to be on the
	 * device side in USB. */
}

int16_t app_get_report_callback(uint8_t interface, uint8_t report_type,
                                uint8_t report_id, const void **report,
                                usb_ep0_data_stage_callback *callback,
                                void **context)
{
	/* This isn't a composite device, so there's no need to check the
	 * interface here. Also, we know that there's only one report for
	 * this device, so there's no need to check report_type or report_id.
	 *
	 * Set report, callback, and context; and the USB stack will send
	 * the report, calling our callback (get_report_callback()) when
	 * it has finished.
	 */
	*report = report_buf;
	*callback = get_report_callback;
	*context = NULL;
	return sizeof(report_buf);
}

int8_t app_set_report_callback(uint8_t interface, uint8_t report_type,
                               uint8_t report_id)
{
	/* To handle Set_Report, call usb_start_receive_ep0_data_stage()
	 * here. See the documentation for HID_SET_REPORT_CALLBACK() in
	 * usb_hid.h. For this device though, there are no output or
	 * feature reports. */
	return -1;
}

uint8_t app_get_idle_callback(uint8_t interface, uint8_t report_id)
{
	return 0;
}

int8_t app_set_idle_callback(uint8_t interface, uint8_t report_id,
                             uint8_t idle_rate)
{
	return -1;
}

int8_t app_get_protocol_callback(uint8_t interface)
{
	return 1;
}

int8_t app_set_protocol_callback(uint8_t interface, uint8_t report_id)
{
	return -1;
}


