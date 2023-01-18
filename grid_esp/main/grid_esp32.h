/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "driver/rmt_encoder.h"

#include "driver/gpio.h"
#include "grid_esp32_pins.h"
#include "esp_check.h"
#include "rom/ets_sys.h" // For ets_printf
#include "esp_efuse.h"


#include "bootloader_random.h"
#include "esp_random.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Read HWCFG register value on ESP32 based module
 *
 * @param[in] void void
 * @return
 *      - 32-bit unsigned value of HWCFG register
 */
uint32_t grid_platform_get_hwcfg();

/**
 * @brief Read CPUID register (mac address) value on ESP32 based module
 *
 * @param[in] void void
 * @return
 *      - 64-bit unsigned value of CPUID register
 */



void grid_platform_printf(char const *fmt, ...);

uint32_t grid_platform_get_id(uint32_t* return_array);

uint32_t grid_platform_get_id(uint32_t* return_array);
uint32_t grid_platform_get_hwcfg();
uint8_t grid_platform_get_random_8();
void grid_platform_delay_ms(uint32_t delay_milliseconds);
uint8_t grid_platform_get_reset_cause();

uint8_t grid_platform_disable_grid_transmitter(uint8_t direction);
uint8_t grid_platform_reset_grid_transmitter(uint8_t direction);
uint8_t grid_platform_enable_grid_transmitter(uint8_t direction);

int32_t grid_platform_usb_serial_write(uint8_t* buffer, uint32_t length);

void* grid_platform_find_actionstring_file(uint8_t page, uint8_t element, uint8_t event_type);
uint16_t grid_platform_get_actionstring_file_size(void* file_pointer);
uint32_t grid_platform_read_actionstring_file_contents(void* file_pointer, char* targetstring);
void grid_platform_delete_actionstring_file(void* file_pointer);
void grid_platform_write_actionstring_file(uint8_t page, uint8_t element, uint8_t event_type, uint8_t* buffer, uint16_t length);



#ifdef __cplusplus
}
#endif
