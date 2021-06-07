/*
 * grid_led.c
 *
 * Created: 4/12/2019 5:27:13 PM
 * Author : SUKU WC
 
 // ===================== WS2812B-Mini SETUP ========================= //

 // The aim is to create a fast WS2812 library optimized for DMA transfers through the SPI interface.
 // Primary goal is to reduce CPU load allowing efficient animations.
 // Memory usage is less of a concern in this implementation.
 //
 // * Each symbol is encoded into a 4bits of serial data.
 //
 // * One LED uses 12 bytes of memory
 // * The reset pulse uses 24 bytes of memory (50us reset)
 // * The reset pulse uses 144 bytes of memory (300us reset) for WS2812-2020
 */ 

#define GRID_LED_RESET_LENGTH 144


// FRAME FORMAT for WS2812B-Mini or WS2812-2020
//
// Memory map =>   | <------------------------------------ FRAME BUFFER ------------------------------------------> |
// Memory map =>   |       | <---------------------------- LED DATA BUFFER ---------------------------------------> |
// Size in bytes=> |24(144)| 4      | 4      | 4      | 4      | 4      | 4      | 4      | 4      | 4      | 4     |
// Frame format => | RESET | LED1:G | LED1:R | LED1:B | LED2:G | LED2:R | LED2:B | LED3:G | LED3:R | LED3:B | IDLE  |
// Frame timing => | 60us  | 10us   | 10us   | 10us   | 10us   | 10us   | 10us   | 10us   | 10us   | 10us   | N/A   |

// this is need for types like uint8_t 
#include "grid_led.h"

// Two symbols are encoded in one byte, the 4 possible combinations are defined here.
// THe NZR code specification for WS2812B-Mini allows a 0.150us deviation from the nominal pulse widths.
//
// * Zero code nominal: 0.40 + 0.85 (us) WS2812B-Mini
// * Zero code actual:  0.31 + 0.93 (us)
//
// * One code nominal: 0.85 + 0.40 (us) WS2812B-Mini
// * One code actual:  0.93 + 0.31 (us)
#define LED_CODE_Z 0x08 // 0000 1000
#define LED_CODE_O 0x0E // 0000 1110


// Reset requires a 50us LOW pulse on the output.
// At the specified 3.2Mbps serial speed this requires 24 consecutive reset bytes (0x00) bytes to be sent out

#define LED_CODE_R 0x00 // 0000 0000




// THE LOOKUP TABLES ARE QUITE SIMPLE, MAYBE CALCULATE THEM ON THE FLY;

uint8_t min_lookup[256] = {
	
	0xfe,0xfc,0xfa,0xf8,0xf6,0xf4,0xf2,0xf0,
	0xee,0xec,0xea,0xe8,0xe6,0xe4,0xe2,0xe0,
	0xde,0xdc,0xda,0xd8,0xd6,0xd4,0xd2,0xd0,
	0xce,0xcc,0xca,0xc8,0xc6,0xc4,0xc2,0xc0,
	0xbe,0xbc,0xba,0xb8,0xb6,0xb4,0xb2,0xb0,
	0xae,0xac,0xaa,0xa8,0xa6,0xa4,0xa2,0xa0,
	0x9e,0x9c,0x9a,0x98,0x96,0x94,0x92,0x90,
	0x8e,0x8c,0x8a,0x88,0x86,0x84,0x82,0x80,
	0x7e,0x7c,0x7a,0x78,0x76,0x74,0x72,0x70,
	0x6e,0x6c,0x6a,0x68,0x66,0x64,0x62,0x60,
	0x5e,0x5c,0x5a,0x58,0x56,0x54,0x52,0x50,
	0x4e,0x4c,0x4a,0x48,0x46,0x44,0x42,0x40,
	0x3e,0x3c,0x3a,0x38,0x36,0x34,0x32,0x30,
	0x2e,0x2c,0x2a,0x28,0x26,0x24,0x22,0x20,
	0x1e,0x1c,0x1a,0x18,0x16,0x14,0x12,0x10,
	0x0e,0x0c,0x0a,0x08,0x06,0x04,0x02,0x00,
	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

uint8_t mid_lookup[256] = {
	
	0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,
	0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
	0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,
	0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
	0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,
	0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
	0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,
	0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
	0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,
	0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
	0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,
	0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
	0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,
	0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
	0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,
	0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
	
	0xfe,0xfc,0xfa,0xf8,0xf6,0xf4,0xf2,0xf0,
	0xee,0xec,0xea,0xe8,0xe6,0xe4,0xe2,0xe0,
	0xde,0xdc,0xda,0xd8,0xd6,0xd4,0xd2,0xd0,
	0xce,0xcc,0xca,0xc8,0xc6,0xc4,0xc2,0xc0,
	0xbe,0xbc,0xba,0xb8,0xb6,0xb4,0xb2,0xb0,
	0xae,0xac,0xaa,0xa8,0xa6,0xa4,0xa2,0xa0,
	0x9e,0x9c,0x9a,0x98,0x96,0x94,0x92,0x90,
	0x8e,0x8c,0x8a,0x88,0x86,0x84,0x82,0x80,
	0x7e,0x7c,0x7a,0x78,0x76,0x74,0x72,0x70,
	0x6e,0x6c,0x6a,0x68,0x66,0x64,0x62,0x60,
	0x5e,0x5c,0x5a,0x58,0x56,0x54,0x52,0x50,
	0x4e,0x4c,0x4a,0x48,0x46,0x44,0x42,0x40,
	0x3e,0x3c,0x3a,0x38,0x36,0x34,0x32,0x30,
	0x2e,0x2c,0x2a,0x28,0x26,0x24,0x22,0x20,
	0x1e,0x1c,0x1a,0x18,0x16,0x14,0x12,0x10,
	0x0e,0x0c,0x0a,0x08,0x06,0x04,0x02,0x00

};

uint8_t max_lookup[256] = {
	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		
	0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,
	0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
	0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,
	0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
	0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,
	0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
	0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,
	0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
	0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,
	0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
	0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,
	0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
	0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,
	0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
	0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,
	0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe
};

uint8_t sine_lookup[256] = {
0x80,0x83,0x86,0x89,0x8c,0x8f,0x92,0x95,
0x98,0x9c,0x9f,0xa2,0xa5,0xa8,0xab,0xae,
0xb0,0xb3,0xb6,0xb9,0xbc,0xbf,0xc1,0xc4,
0xc7,0xc9,0xcc,0xce,0xd1,0xd3,0xd5,0xd8,
0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,
0xea,0xeb,0xed,0xef,0xf0,0xf2,0xf3,0xf4,
0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfb,0xfc,
0xfd,0xfd,0xfe,0xfe,0xfe,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xfe,0xfe,0xfd,0xfd,
0xfc,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,0xf6,
0xf5,0xf4,0xf2,0xf1,0xef,0xee,0xec,0xeb,
0xe9,0xe7,0xe5,0xe3,0xe1,0xdf,0xdd,0xdb,
0xd9,0xd7,0xd4,0xd2,0xcf,0xcd,0xca,0xc8,
0xc5,0xc3,0xc0,0xbd,0xba,0xb8,0xb5,0xb2,
0xaf,0xac,0xa9,0xa6,0xa3,0xa0,0x9d,0x9a,
0x97,0x94,0x91,0x8e,0x8a,0x87,0x84,0x81,
0x7e,0x7b,0x78,0x75,0x71,0x6e,0x6b,0x68,
0x65,0x62,0x5f,0x5c,0x59,0x56,0x53,0x50,
0x4d,0x4a,0x47,0x45,0x42,0x3f,0x3c,0x3a,
0x37,0x35,0x32,0x30,0x2d,0x2b,0x28,0x26,
0x24,0x22,0x20,0x1e,0x1c,0x1a,0x18,0x16,
0x14,0x13,0x11,0x10,0xe,0xd,0xb,0xa,
0x9,0x8,0x7,0x6,0x5,0x4,0x3,0x3,
0x2,0x2,0x1,0x1,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x2,0x2,
0x3,0x4,0x4,0x5,0x6,0x7,0x8,0x9,
0xb,0xc,0xd,0xf,0x10,0x12,0x14,0x15,
0x17,0x19,0x1b,0x1d,0x1f,0x21,0x23,0x25,
0x27,0x2a,0x2c,0x2e,0x31,0x33,0x36,0x38,
0x3b,0x3e,0x40,0x43,0x46,0x49,0x4c,0x4f,
0x51,0x54,0x57,0x5a,0x5d,0x60,0x63,0x67,
0x6a,0x6d,0x70,0x73,0x76,0x79,0x7c,0x80};

/** Get pointer of buffer array */
uint8_t* grid_led_lowlevel_get_frame_buffer_pointer(struct grid_led_model* mod){
	
	return mod->led_frame_buffer;
}

/** Get led buffer size */
uint32_t grid_led_lowlevel_get_frame_buffer_size(struct grid_led_model* mod){
	
	return mod->led_frame_buffer_size;
}

/** Set color of a particular LED in the frame buffer */
uint8_t grid_led_lowlevel_set_color(struct grid_led_model* mod, uint32_t led_index, uint16_t led_r, uint16_t led_g, uint16_t led_b){
	
	if (led_r>255){
		led_r = 255;
	}
	if (led_g>255){
		led_g = 255;
	}
	if (led_b>255){
		led_b = 255;
	}

	
	//if index is valid
	if (led_index<mod->led_number){

		// green
		if (mod->led_frame_buffer_usable[led_index*3 + 0] != grid_led_color_code[led_g]){
			mod->led_lowlevel_changed[led_index] = 1;
			mod->led_lowlevel_gre[led_index] = led_g;
			mod->led_frame_buffer_usable[led_index*3 + 0] = grid_led_color_code[led_g];
		}

		// red
		if (mod->led_frame_buffer_usable[led_index*3 + 1] != grid_led_color_code[led_r]){
			mod->led_lowlevel_changed[led_index] = 1;
			mod->led_lowlevel_red[led_index] = led_r;
			mod->led_frame_buffer_usable[led_index*3 + 1] = grid_led_color_code[led_r];
		}
		
		// blue
		if (mod->led_frame_buffer_usable[led_index*3 + 2] != grid_led_color_code[led_b]){
			mod->led_lowlevel_changed[led_index] = 1;
			mod->led_lowlevel_blu[led_index] = led_b;
			mod->led_frame_buffer_usable[led_index*3 + 2] = grid_led_color_code[led_b];
		}


		return 0;
		
	}
	else{
		
		return -1;		
		
	}
}





void grid_led_lowlevel_hardware_transfer_complete_cb(struct _dma_resource *resource){
	

	grid_led_hardware_transfer_done = 1;
	
	
}


void grid_led_buffer_init(struct grid_led_model* mod, uint32_t length){
	
	mod->led_number = length;
	
	uint32_t last_tick_realtime = 0;
	
	
		
	// Allocating memory for the frame buffer
	mod->led_frame_buffer_size = (GRID_LED_RESET_LENGTH + mod->led_number*3*4);
	mod->led_frame_buffer = (uint8_t*) malloc(mod->led_frame_buffer_size * sizeof(uint8_t));
	mod->led_frame_buffer_usable = (uint32_t*) &mod->led_frame_buffer[GRID_LED_RESET_LENGTH];
	

	// Allocating memory for the smart buffer (2D array)
		
	
	mod->led_smart_buffer = (struct LED_layer*) malloc(mod->led_number * GRID_LED_LAYER_NUMBER * sizeof(struct LED_layer));
	
	if(mod->led_frame_buffer==NULL || mod->led_smart_buffer==NULL){
		while(1){
			//MALLOC FAILED			
		}		
		
	}


	// Allocating memory for low level color buffer for reporting

	mod->led_lowlevel_red = (uint8_t*) malloc(mod->led_number * sizeof(uint8_t));
	mod->led_lowlevel_gre = (uint8_t*) malloc(mod->led_number * sizeof(uint8_t));
	mod->led_lowlevel_blu = (uint8_t*) malloc(mod->led_number * sizeof(uint8_t));
	mod->led_lowlevel_changed = (uint8_t*) malloc(mod->led_number * sizeof(uint8_t));
	
	if(mod->led_lowlevel_changed == NULL || mod->led_lowlevel_red == NULL || mod->led_lowlevel_gre == NULL || mod->led_lowlevel_blu == NULL){
		while(1){
			//MALLOC FAILED			
		}		
		
	}


	// Fill the first 24 bytes with the rr_code (reset)
	// This memory is essentially wasted but allows the entire frame to be sent out using DMA

	for (uint8_t i = 0; i<GRID_LED_RESET_LENGTH; i++){
		mod->led_frame_buffer[i] = LED_CODE_R;
	}
		
		
	// Fill the rest of the buffer with rgb=(0,0,0);
	for (uint32_t i = 0; i<mod->led_number; i++){
		grid_led_lowlevel_set_color(mod,i,0,0,0);
	}

	// Clear Changed buffer
	for (uint8_t i = 0; i<mod->led_number; i++){
		mod->led_lowlevel_changed[i] = 0;
		mod->led_lowlevel_red[i] = 0;
		mod->led_lowlevel_gre[i] = 0;
		mod->led_lowlevel_blu[i] = 0;
	}
	

	// DEFAULT CONFIG 
	for(uint8_t i = 0; i<mod->led_number; i++){
			
		for(uint8_t j = 0; j<GRID_LED_LAYER_NUMBER; j++){

			grid_led_set_min(mod,i, j, 0x00, 0x00, 0x00);
			grid_led_set_mid(mod,i, j, 0x00, 0x00, 0x00);
			grid_led_set_max(mod,i, j, 0x00, 0x00, 0x00);
				
			grid_led_set_frequency(mod,i, j, 0);
			grid_led_set_phase(mod, i, j, 0);
			grid_led_set_shape(mod, i, j, 0);

			grid_led_set_timeout(mod, i, j, 0);

		}

	}


}


void grid_led_hardware_init(struct grid_led_model* mod){
	
	spi_m_dma_get_io_descriptor(&GRID_LED, &mod->hardware_io_descriptor);
	spi_m_dma_register_callback(&GRID_LED, SPI_M_DMA_CB_TX_DONE, grid_led_lowlevel_hardware_transfer_complete_cb);
	

		
}

void grid_led_startup_animation(struct grid_led_model* mod){

	return;

	uint8_t grid_module_reset_cause = hri_rstc_read_RCAUSE_reg(RSTC);
	
	uint8_t color_r   = 1;
	uint8_t color_g   = 1;
	uint8_t color_b   = 1;
	uint8_t s		  = 1;
	
	if (grid_module_reset_cause == RESET_REASON_WDT){
		
		color_r = 1;
		color_g = 0;
		color_b = 0;
		s= 2;

	}else if (grid_module_reset_cause == RESET_REASON_SYST){
		
		color_r = 0;
		color_g = 0;
		color_b = 1;
		s= 2;
		
	}
	
	
	
	for (uint8_t i = 0; i<255; i++){
	
		// SEND DATA TO LEDs
		

		for (uint8_t j=0; j<mod->led_number; j++){
			//grid_led_lowlevel_set_color(i, 0, 255, 0);
			grid_led_lowlevel_set_color(mod, j, (color_r*i*s%256)/2, (color_g*i*s%256)/2, (color_b*i*s%256)/2); // This is not an alert, this is low level shit
			
			
		}
		
		grid_led_lowlevel_hardware_start_transfer_blocking(mod);

		
		delay_us(500);
		
	}
	

	

	
}



/** Initialize led buffer for a given number of LEDs */
uint8_t grid_led_lowlevel_init(struct grid_led_model* mod, uint8_t num){
	

	// Generate the lookup table for fast rendering;

	// Generating the lookup table for 2 symbols per byte encoding
	//
	//  * Reading intensity values from lookup table reduces the CPU cycles used for calculating the fram buffer.
	//  * This is especially useful for fast animations using large number of LEDs.
	//
	//	* Calculating the lookup table during startup saves program memory at the expense of RAM
	//  * Memory usage: 256*32bit -> 1kbyte
	//
	//	* Input: 8bit intensity;
	//  * Output: 8bit intensity encoded into 4byte long NZR intensity code;
	//
	//	* The output can be sent directly to the RGB chip through SPI or GPIO.
	//  * Green Red and Blue intensity values are sent to the LEDs in the given order.
	//  * The output should have a baudrate of 3.2Mbps resulting in an effective symbol rate of 800k sym per sec
	//
	//	* MSB first (WS2812B-Mini)
	//	* G7 ... G0 R7 ... R0 B7 ... B0 ;

	for(uint16_t i=0; i<256; i++){
	
		uint32_t temp = 0;
	
		temp |= (i/1%2)   ? (LED_CODE_O<<24) : (LED_CODE_Z<<24);
		temp |= (i/2%2)   ? (LED_CODE_O<<28) : (LED_CODE_Z<<28);
	
		temp |= (i/4%2)   ? (LED_CODE_O<<16) : (LED_CODE_Z<<16);
		temp |= (i/8%2)   ? (LED_CODE_O<<20) : (LED_CODE_Z<<20);
	
		temp |= (i/16%2)  ? (LED_CODE_O<<8)  : (LED_CODE_Z<<8);
		temp |= (i/32%2)  ? (LED_CODE_O<<12) : (LED_CODE_Z<<12);
		temp |= (i/64%2)  ? (LED_CODE_O<<0)  : (LED_CODE_Z<<0);
		temp |= (i/128%2) ? (LED_CODE_O<<4)  : (LED_CODE_Z<<4);
	
		grid_led_color_code[i] = temp;
	}
	
	grid_led_buffer_init(mod, num);		
	grid_led_hardware_init(mod);
	
	grid_led_startup_animation(mod);
	
	return 0;
}


uint32_t grid_led_get_led_number(struct grid_led_model* mod){

	return mod->led_number;

}

/** ================== ANIMATION ==================  */


void grid_led_tick(struct grid_led_model* mod){

	for (uint8_t j=0; j<mod->led_number; j++){
					
		for(uint8_t i=0; i<GRID_LED_LAYER_NUMBER; i++){
			
			struct LED_layer* ledbuf = &mod->led_smart_buffer[j+(mod->led_number*i)];
			
			if (ledbuf->timeout != 0){
				
				ledbuf->timeout--;

				if (ledbuf->timeout == 0){
					ledbuf->fre = 0;
				}

			}

			ledbuf->pha += ledbuf->fre; //PHASE + = FREQUENCY		

		}	
	}
	
}

void grid_led_set_alert(struct grid_led_model* mod, uint8_t r, uint8_t g, uint8_t b, uint16_t duration){

	//printf("ALERT\r\n");

	for (uint8_t i = 0; i<mod->led_number; i++){

		grid_led_set_color(mod, i, GRID_LED_LAYER_ALERT, r, g, b);

		// just to make sure that minimum is 0
		grid_led_set_min(mod, i, GRID_LED_LAYER_ALERT, 0, 0, 0);

		grid_led_set_shape(mod, i, GRID_LED_LAYER_ALERT, 0);

		grid_led_set_timeout(mod, i, GRID_LED_LAYER_ALERT, duration);
		grid_led_set_phase(mod, i, GRID_LED_LAYER_ALERT, (uint8_t)duration);
		grid_led_set_frequency(mod, i, GRID_LED_LAYER_ALERT, -1);

	}

}

void grid_led_set_alert_frequency(struct grid_led_model* mod, uint8_t frequency){

	//printf("ALERT\r\n");

	for (uint8_t i = 0; i<mod->led_number; i++){
		grid_led_set_frequency(mod, i, GRID_LED_LAYER_ALERT, frequency);
	}

}

void grid_led_set_alert_phase(struct grid_led_model* mod, uint8_t phase){

	//printf("ALERT\r\n");

	for (uint8_t i = 0; i<mod->led_number; i++){
		grid_led_set_phase(mod, i, GRID_LED_LAYER_ALERT, phase);
	}

}

void grid_led_set_color(struct grid_led_model* mod, uint8_t num, uint8_t layer, uint8_t r, uint8_t g, uint8_t b){
	
	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){	
		
		grid_led_set_min(mod, num, layer, r/20, g/20, b/20);				
		grid_led_set_mid(mod, num, layer, r/2, g/2, b/2);
		grid_led_set_max(mod, num, layer, r, g, b);
	}

}


void grid_led_set_min(struct grid_led_model* mod, uint8_t num, uint8_t layer, uint8_t r, uint8_t g, uint8_t b){
	
	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){	

		mod->led_smart_buffer[num+(mod->led_number*layer)].color_min.r = r;
		mod->led_smart_buffer[num+(mod->led_number*layer)].color_min.g = g;
		mod->led_smart_buffer[num+(mod->led_number*layer)].color_min.b = b;	
	}
}

void grid_led_set_mid(struct grid_led_model* mod, uint8_t num, uint8_t layer, uint8_t r, uint8_t g, uint8_t b){
	
	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){	

		mod->led_smart_buffer[num+(mod->led_number*layer)].color_mid.r = r;
		mod->led_smart_buffer[num+(mod->led_number*layer)].color_mid.g = g;
		mod->led_smart_buffer[num+(mod->led_number*layer)].color_mid.b = b;	
	}
}

void grid_led_set_max(struct grid_led_model* mod, uint8_t num, uint8_t layer, uint8_t r, uint8_t g, uint8_t b){

	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){	

		mod->led_smart_buffer[num+(mod->led_number*layer)].color_max.r = r;
		mod->led_smart_buffer[num+(mod->led_number*layer)].color_max.g = g;
		mod->led_smart_buffer[num+(mod->led_number*layer)].color_max.b = b;	
	}
}

void grid_led_set_phase(struct grid_led_model* mod, uint8_t num, uint8_t layer, uint8_t val){
	
	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){	

		mod->led_smart_buffer[num+(mod->led_number*layer)].pha = val;
	}
}

uint8_t grid_led_get_phase(struct grid_led_model* mod, uint8_t num, uint8_t layer){
	
	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){	

		return mod->led_smart_buffer[num+(mod->led_number*layer)].pha;
	}
}

void grid_led_set_frequency(struct grid_led_model* mod, uint8_t num, uint8_t layer, uint8_t val){

	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){	
	
		mod->led_smart_buffer[num+(mod->led_number*layer)].fre = val;
	}
}

void grid_led_set_shape(struct grid_led_model* mod, uint8_t num, uint8_t layer, uint8_t val){
	
	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){	

		mod->led_smart_buffer[num+(mod->led_number*layer)].sha = val;
	}

}

void grid_led_set_timeout(struct grid_led_model* mod, uint8_t num, uint8_t layer, uint16_t val){
	
	if (num<mod->led_number && layer<GRID_LED_LAYER_NUMBER){

		mod->led_smart_buffer[num+(mod->led_number*layer)].timeout = val;
	}

}


void grid_led_lowlevel_render(struct grid_led_model* mod, uint32_t num){
	
	
	uint32_t mix_r = 0;
	uint32_t mix_g = 0;
	uint32_t mix_b = 0;
	
	// RENDER & SUM ALL LAYERS PER LED
	for (uint8_t i = 0; i<GRID_LED_LAYER_NUMBER; i++){
		
		uint8_t layer = i;

		uint8_t phase = mod->led_smart_buffer[num+(mod->led_number*layer)].pha;

		/* SHAPES
		
		0:  ramp up
		1:  ramp down
		2:  square
		3:  sine

		*/


		uint8_t intensity = phase;


		uint8_t shape = mod->led_smart_buffer[num+(mod->led_number*layer)].sha;

		if (shape == 0){
			intensity = phase;
		}
		else if (shape == 1){
			intensity = 255 - phase;
		}
		else if (shape == 2){
			intensity = (phase<128)*255;
		}
		else if (shape == 3){
			intensity = sine_lookup[phase];
		}
		
				
		uint8_t min_r = mod->led_smart_buffer[num+(mod->led_number*layer)].color_min.r;
		uint8_t min_g = mod->led_smart_buffer[num+(mod->led_number*layer)].color_min.g;
		uint8_t min_b = mod->led_smart_buffer[num+(mod->led_number*layer)].color_min.b;
		uint8_t min_a = min_lookup[intensity];
	
		uint8_t mid_r = mod->led_smart_buffer[num+(mod->led_number*layer)].color_mid.r;
		uint8_t mid_g = mod->led_smart_buffer[num+(mod->led_number*layer)].color_mid.g;
		uint8_t mid_b = mod->led_smart_buffer[num+(mod->led_number*layer)].color_mid.b;
		uint8_t mid_a = mid_lookup[intensity];
		
		uint8_t max_r = mod->led_smart_buffer[num+(mod->led_number*layer)].color_max.r;
		uint8_t max_g = mod->led_smart_buffer[num+(mod->led_number*layer)].color_max.g;
		uint8_t max_b = mod->led_smart_buffer[num+(mod->led_number*layer)].color_max.b;
		uint8_t max_a = max_lookup[intensity];
				
		mix_r += min_r*min_a + mid_r*mid_a + max_r*max_a;
		mix_g += min_g*min_a + mid_g*mid_a + max_g*max_a;
		mix_b += min_b*min_a + mid_b*mid_a + max_b*max_a;
	}
	
// 
// mix_r = (mix_r)/2/3/256;
// mix_g = (mix_g)/2/3/256;
// mix_b = (mix_b)/2/3/256;

	mix_r = (mix_r)/2/256;
	mix_g = (mix_g)/2/256;
	mix_b = (mix_b)/2/256;
				
	grid_led_lowlevel_set_color(mod, num, mix_r, mix_g, mix_b);
	
}


void grid_led_lowlevel_render_all(struct grid_led_model* mod){
	
	for (uint32_t i=0; i<mod->led_number; i++){
		
		grid_led_lowlevel_render(mod, i);
	}
	
}


void grid_led_lowlevel_hardware_start_transfer_blocking(struct grid_led_model* mod){
	
	// SEND DATA TO LEDs
	grid_led_hardware_transfer_done = 0;
	spi_m_dma_enable(&GRID_LED);
			
	io_write(mod->hardware_io_descriptor, grid_led_lowlevel_get_frame_buffer_pointer(mod), grid_led_lowlevel_get_frame_buffer_size(mod));
	while(grid_led_hardware_transfer_done!=1){
			
	}
	
}

void grid_led_lowlevel_hardware_start_transfer (struct grid_led_model* mod){
	
	// SEND DATA TO LEDs
	grid_led_hardware_transfer_done = 0;
	spi_m_dma_enable(&GRID_LED);
			
	io_write(mod->hardware_io_descriptor, grid_led_lowlevel_get_frame_buffer_pointer(mod), grid_led_lowlevel_get_frame_buffer_size(mod));

}

uint8_t grid_led_lowlevel_hardware_is_transfer_completed(struct grid_led_model* mod){
	

	return grid_led_hardware_transfer_done;
	
}




uint8_t grid_led_lowlevel_change_count(struct grid_led_model* mod){

	uint8_t count = 0; 

	for(uint8_t i=0; i<mod->led_number; i++){
		if (mod->led_lowlevel_changed[i] != 0){
			count++;
		}
	}

	return count;

}


uint16_t grid_led_lowlevel_change_report_length(struct grid_led_model* mod){

	return grid_led_lowlevel_change_count(mod) * 8;
}

uint16_t grid_led_lowlevel_change_report(struct grid_led_model* mod, uint16_t maxlength, uint8_t* output){

	uint16_t length = 0;

	for(uint8_t i=0; i<mod->led_number; i++){

		if (mod->led_lowlevel_changed[i] != 0){

			if (length + 8 <= maxlength){

				grid_msg_set_parameter(&output[length], 0, 2, i, NULL);
				grid_msg_set_parameter(&output[length], 2, 2, mod->led_lowlevel_red[i], NULL);
				grid_msg_set_parameter(&output[length], 4, 2, mod->led_lowlevel_gre[i], NULL);
				grid_msg_set_parameter(&output[length], 6, 2, mod->led_lowlevel_blu[i], NULL);
				
				mod->led_lowlevel_changed[i] = 0;

				length+=8;
			}
			else{
				break;
			}
		}
	}

	return length; 
}