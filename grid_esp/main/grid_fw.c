/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"

#include <string.h>

#include "driver/spi_master.h"
#include "rom/ets_sys.h" // For ets_printf

#include "driver/gpio.h"
#include "esp_flash.h"
#include "esp_task_wdt.h"


#include "pico_firmware.h"
#include "grid_esp32_adc.h"
#include "grid_esp32_led.h"


#define LED_TASK_PRIORITY 2
#define LED_TASK_PRIORITY 2


#include "driver/ledc.h"
#include <esp_timer.h>

#include "grid_esp32.h"
#include "grid_esp32_swd.h"

#include "esp_log.h"
#include "esp_check.h"
#include "rom/ets_sys.h" // For ets_printf


#include "../../grid_common/grid_protocol.h"
#include "../../grid_common/grid_ain.h"
#include "../../grid_common/grid_led.h"
#include "../../grid_common/grid_sys.h"
#include "../../grid_common/grid_msg.h"
#include "../../grid_common/grid_buf.h"
#include "../../grid_common/grid_port.h"
#include "../../grid_common/grid_usb.h"

#include "../../grid_common/grid_lua_api.h"


#include "../../grid_common/lua-5.4.3/src/lua.h"
#include "../../grid_common/lua-5.4.3/src/lualib.h"
#include "../../grid_common/lua-5.4.3/src/lauxlib.h"


void app_main(void)
{

    lua_State* L = luaL_newstate();

    luaL_openlibs(L);

    uint32_t memusage = lua_gc(L, LUA_GCCOUNT)*1024 + lua_gc(L, LUA_GCCOUNTB);
    printf("LUA mem usage: %ld\r\n", memusage);

    static const char *TAG = "main";

    grid_platform_get_hwcfg();
    uint32_t id_array[4] = {};
    grid_platform_get_id(id_array);

    // GRID_MODULE_INIT (based on hwcfg)

    ets_printf("RANDOM: %d %d %d %d\r\n", grid_platform_get_random_8(), grid_platform_get_random_8(), grid_platform_get_random_8(), grid_platform_get_random_8());


    ets_printf("LED INIT ...\r\n");

    grid_led_init(&grid_led_state, 16);
    grid_msg_init(&grid_msg_state);

    ets_printf("LED INIT DONE\r\n");


    vTaskDelay(100);
    
    grid_esp32_swd_pico_pins_init(GRID_ESP32_PINS_RP_SWCLK, GRID_ESP32_PINS_RP_SWDIO, GRID_ESP32_PINS_RP_CLOCK);
    grid_esp32_swd_pico_clock_init(LEDC_TIMER_0, LEDC_CHANNEL_0);
    grid_esp32_swd_pico_program_sram(GRID_ESP32_PINS_RP_SWCLK, GRID_ESP32_PINS_RP_SWDIO, ___grid_pico_build_main_main_bin, ___grid_pico_build_main_main_bin_len);


    ets_printf("Testing External Libraries:\r\n");
    ets_printf("Version: %d %d %d\r\n", GRID_PROTOCOL_VERSION_MAJOR, GRID_PROTOCOL_VERSION_MINOR, GRID_PROTOCOL_VERSION_PATCH );
    ets_printf("grid_ain_abs test -7 -> %d\r\n", grid_ain_abs(-7));

    struct grid_buffer buf;

    grid_buffer_init(&buf, 1000);
    grid_port_init_all();
    grid_usb_midi_init();

    SemaphoreHandle_t signaling_sem = xSemaphoreCreateBinary();


    grid_sys_init(&grid_sys_state);

    ets_printf("GRID_SYS_TEST %d\r\n", grid_sys_get_hwcfg(&grid_sys_state));
    grid_platform_delay_ms(10);

    TaskHandle_t adc_task_hdl;


    //Create the class driver task
    xTaskCreatePinnedToCore(grid_esp32_adc_task,
                            "adc",
                            4096,
                            (void *)signaling_sem,
                            LED_TASK_PRIORITY,
                            &adc_task_hdl,
                            0);


    TaskHandle_t led_task_hdl;

    //Create the class driver task
    xTaskCreatePinnedToCore(grid_esp32_led_task,
                            "led",
                            4096,
                            (void *)signaling_sem,
                            LED_TASK_PRIORITY,
                            &led_task_hdl,
                            0);



}
