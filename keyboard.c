#include "keyboard.h"

#include "usb/include/usb.h"
#include "usb/include/usb_hid.h"
#include "usb_config.h"

typedef enum keyboard_state {
    K_INIT_STATE,
    K_WAIT_STATE,
} keyboard_state;

struct {
    int count;
    int pressed;
} k_data;

bool keyboard_state_machine() {
    static keyboard_state STATE = K_INIT_STATE;
    switch(STATE) {
    case K_INIT_STATE: {
        usb_init();
        k_data.count = 0;
        k_data.pressed = 0;
        STATE = K_WAIT_STATE;
        break;
    }
    case K_WAIT_STATE: {
        if(k_data.count <= 0) {
            if(usb_is_configured() &&
                !usb_in_endpoint_halted(1) &&
                !usb_in_endpoint_busy(1)) {

                unsigned char *buf = usb_get_in_buffer(1);
                buf[0] = 0x0;
                buf[1] = 0x0;
                if(k_data.pressed) 
                    buf[2] = 0x0;
                else
                    buf[2] = 0x4;
                k_data.pressed = !k_data.pressed;
                buf[3] = 0x0;
                buf[4] = 0x0;
                buf[5] = 0x0;
                buf[6] = 0x0;
                buf[7] = 0x0;
                usb_send_in_buffer(1, 8);
            }
            k_data.count = 100000;
        }
        k_data.count --;
        usb_service();
        break;
    }
    }
    return false;
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


