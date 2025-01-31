/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "grid_esp32_usb.h"




#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "driver/gpio.h"


static const char *TAG = "USB example";

static uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];

volatile uint16_t grid_usb_rx_double_buffer_index = 0;


void tud_midi_rx_cb(uint8_t itf){

    //ets_printf("MIDI RX: %d\n", itf);

    // The MIDI interface always creates input and output port/jack descriptors
    // regardless of these being used or not. Therefore incoming traffic should be read
    // (possibly just discarded) to avoid the sender blocking in IO
    uint8_t packet[4];
    bool read = false;

    while (tud_midi_available()) {
        read = tud_midi_packet_read(packet);
        if (read) {
            //ets_printf("Read, Data: %02x %02x %02x %02x\r\n", packet[0], packet[1], packet[2], packet[3]);
        
        
            uint8_t channel = packet[1] & 0x0f;
            uint8_t command = packet[1] & 0xf0;
            uint8_t param1 = packet[2];
            uint8_t param2 = packet[3];

            //grid_port_debug_printf("decoded: %d %d %d %d", channel, command, param1, param2);
            
            struct grid_midi_event_desc midi_ev;

            midi_ev.byte0 = channel;
            midi_ev.byte1 = command;
            midi_ev.byte2 = param1;
            midi_ev.byte3 = param2;

            if ((midi_ev.byte0 == 8 || midi_ev.byte0 == 10 || midi_ev.byte0 == 12) && midi_ev.byte1 == 240){
                // if element's timer clock source is midi then decrement timer_helper
                grid_platform_sync1_pulse_send();

            }



		    grid_midi_rx_push(midi_ev);
        
        }
    }
    
}



void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);


    for (uint16_t i=0; i<rx_size; i++){

		GRID_PORT_H->rx_double_buffer[grid_usb_rx_double_buffer_index] = buf[i];

		
		grid_usb_rx_double_buffer_index++;
		grid_usb_rx_double_buffer_index%=GRID_DOUBLE_BUFFER_RX_SIZE;

	}

    //ESP_LOGI(TAG, "Data from channel %d len: %d", itf, rx_size);

}

static uint8_t DRAM_ATTR usb_tx_ready = 0;

void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rts = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);

    usb_tx_ready = 1;
}



void tud_cdc_tx_complete_cb(uint8_t itf){
    //ets_printf("CDC TXC\r\n");
    usb_tx_ready = 1;
}

int32_t grid_platform_usb_serial_write(char* buffer, uint32_t length){

    //tinyusb_cdcacm_write_flush(0, pdMS_TO_TICKS(10));
    if (usb_tx_ready == 1){

        usb_tx_ready = 0;
    
   
        uint32_t queued = tinyusb_cdcacm_write_queue(0, (const uint8_t*) buffer, length);

        if (queued==0){
            usb_tx_ready = 1;
        }       

        if (queued != length){
            //ets_printf("CDC QUEUE ERROR: %d %d\r\n", queued, length);
            //tinyusb_cdcacm_write_flush(0, pdMS_TO_TICKS(1));
        }

        tinyusb_cdcacm_write_flush(0, 0);
    }
    else{

        //ets_printf("CDC SKIP\r\n");
        tinyusb_cdcacm_write_flush(0, 0);

    }



    /*
    char temp[length+1];
    temp[length] = '\0';
    for (uint16_t i=0; i<length; i++){

        temp[i] = buffer[i];
    }
    ets_printf("CDC: %d %s\r\n", queued, temp);
    */

    //tinyusb_cdcacm_write_flush(0, pdMS_TO_TICKS(1000));


    return 1;
}


// =========================== HID ======================== //

/**
 * @brief HID report descriptor
 *
 * In this example we implement Keyboard + Mouse HID device,
 * so we must define both report descriptors
 */
const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD) ),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE) )
};

/********* TinyUSB HID callbacks ***************/

// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
    return hid_report_descriptor;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
}



// Interface counter
enum interface_count {
#if CFG_TUD_CDC
    ITF_NUM_CDC_NOTIFY,
    ITF_NUM_CDC_DATA,
#endif
#if CFG_TUD_MIDI
    ITF_NUM_MIDI,
    ITF_NUM_MIDI_STREAMING,
#endif
#if CFG_TUD_HID
    ITF_NUM_HID,
#endif
    ITF_COUNT
};

// USB Endpoint numbers
enum usb_endpoints {
    // Available USB Endpoints: 5 IN/OUT EPs and 1 IN EP
    EP_EMPTY = 0,
#if CFG_TUD_CDC
    EPNUM_CDC_NOTIFY,
    EPNUM_CDC_DATA,
#endif
#if CFG_TUD_MIDI
    EPNUM_MIDI,
#endif
#if CFG_TUD_HID
    EPNUM_HID,
#endif
    ENDPOINT_COUNT
};

//#EPNUM_MIDI_IN
//#EPNUM_MIDI_OUT


/** TinyUSB descriptors **/

#define TUSB_DESCRIPTOR_TOTAL_LEN (TUD_CONFIG_DESC_LEN + CFG_TUD_CDC * TUD_CDC_DESC_LEN + CFG_TUD_MIDI * TUD_MIDI_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

/**
 * @brief String descriptor
 */
static const char* s_str_desc[6] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "Intech Studio",             // 1: Manufacturer
    "Grid",      // 2: Product
    "123456",              // 3: Serials, should use chip ID
    "Intech Grid MIDI device", // 4: MIDI
    "Intech Grid CDC device",  // 5: CDC
};

/**
 * @brief Configuration descriptor
 *
 * This is a simple configuration descriptor that defines 1 configuration and a MIDI interface
 */
static uint8_t s_cfg_desc[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_COUNT, 0, TUSB_DESCRIPTOR_TOTAL_LEN, 0, 500),


    #if CFG_TUD_CDC
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_NOTIFY, 5, (0x80 | EPNUM_CDC_NOTIFY), 64, EPNUM_CDC_DATA, (0x80 | EPNUM_CDC_DATA), 64),
    #endif

    #if CFG_TUD_MIDI
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 4, EPNUM_MIDI, (0x80 | EPNUM_MIDI), 64),
    #endif   
  
    #if CFG_TUD_HID
    // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, false, sizeof(hid_report_descriptor), (0x80 | EPNUM_HID), 16, 10),
    #endif      


};


#if CFG_TUD_MIDI

static void midi_task_read_example(void *arg)
{
    // The MIDI interface always creates input and output port/jack descriptors
    // regardless of these being used or not. Therefore incoming traffic should be read
    // (possibly just discarded) to avoid the sender blocking in IO
    uint8_t packet[4];
    bool read = false;
    for (;;) {
        vTaskDelay(1);
        while (tud_midi_available()) {
            read = tud_midi_packet_read(packet);
            if (read) {
                ESP_LOGI(TAG, "Read - Time (ms since boot): %lld, Data: %02hhX %02hhX %02hhX %02hhX",
                         esp_timer_get_time(), packet[0], packet[1], packet[2], packet[3]);
            }
        }
    }
}


#endif 


void grid_esp32_usb_init(){

    tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL, // If device_descriptor is NULL, tinyusb_driver_install() will use Kconfig
        .string_descriptor = s_str_desc,
        .external_phy = false,
        .configuration_descriptor = s_cfg_desc,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 64,
        .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));

    // END OF USB

}

void grid_esp32_usb_task(void *arg)
{


    SemaphoreHandle_t signaling_sem = (SemaphoreHandle_t)arg;


    ESP_LOGI(TAG, "Test Print From USB");
    
    grid_esp32_usb_init();


    ESP_LOGI(TAG, "Deinit USB");
    vTaskSuspend(NULL);
}
