/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"

#include "main.h"
#include "batt_soc/batt_soc.h"
#include "adc/adc.h"

const char *TAG = "MICROMOUSE";

void app_main(void)
{
    sensor_init();

    while (1) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, V_SENSE, &adc_raw[4]));
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc2_cali_handle, adc_raw[4], &voltage[4]));
        
        int batt_voltage = batt_volts(voltage[4]);
        ESP_LOGI(TAG, "\nBattery Pack Voltage: %d mV (%.1f %%)", batt_voltage, 100.0*batt_soc(batt_voltage));

        for(int i=0; i<4; i++){
            get_prox(sensors[i], &adc_raw[i], &voltage[i]);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));

    }

}