#ifndef BLUETOOTH
    #define BLUETOOTH

    #include <stdint.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include <inttypes.h>
    #include "nvs.h"
    #include "nvs_flash.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_log.h"
    #include "esp_bt.h"
    #include "esp_bt_main.h"
    #include "esp_gap_bt_api.h"
    #include "esp_bt_device.h"
    #include "esp_spp_api.h"
    #include "freertos/semphr.h"

    #define SPP_TAG "MICROMOUSE_SPP_ACCEPTOR"
    #define SPP_SERVER_NAME "SPP_SERVER"
    #define EXAMPLE_DEVICE_NAME "MICROMOUSE"
    #define SPP_SHOW_DATA 0
    #define SPP_SHOW_SPEED 1
    #define SPP_SHOW_MODE SPP_SHOW_DATA    /*Choose show mode: show data or speed*/

    void bluetooth_log(char* data);

    void bluetooth_init(void);

    extern uint32_t connection_handle;
#endif