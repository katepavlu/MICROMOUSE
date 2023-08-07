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
#include "icm20602/icm20602.h"

const char *TAG = "MICROMOUSE";

const float gravity = 9.81;
const int max_signed_16 = 32767;
const float pi = 3.141592653589;

imu_t imu;

#define SERIAL_STUDIO

void app_main(void)
{
    #ifdef SERIAL_STUDIO
        esp_log_level_set("MICROMOUSE", ESP_LOG_NONE); //disable logging
    #endif

    sensor_init();
    imu_init();

    while (1) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, V_SENSE, &adc_raw[4]));
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc2_cali_handle, adc_raw[4], &voltage[4]));
        
        int batt_voltage = batt_volts(voltage[4]);
        double batt_charge = batt_soc(batt_voltage);
        ESP_LOGI(TAG, "\nBattery Pack Voltage: %d mV (%.1f %%)", batt_voltage, batt_charge*100.0);

        imu_read(&imu);

        for(int i=0; i<4; i++){
            get_prox(sensors[i], adc_raw, voltage);
        }

        #ifndef SERIAL_STUDIO
            vTaskDelay(pdMS_TO_TICKS(1000));
        #endif
        #ifdef SERIAL_STUDIO
        #if 0
            printf("/*%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f*/", 
            batt_voltage/1000.0, batt_charge, 
            voltage[0]/1000.0, voltage[1]/1000.0, voltage[2]/1000.0, voltage[3]/1000.0, 
            imu.acc.x, imu.acc.y, imu.acc.z,
            imu.omega.x, imu.omega.y, imu.omega.z,
            imu.temp);
            vTaskDelay(pdMS_TO_TICKS(100));
            */
        #endif

            printf("/*%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f*/\n", 
            batt_voltage/1000.0, batt_charge*100.0,
            voltage[0]/1000.0, voltage[1]/1000.0, voltage[2]/1000.0, voltage[3]/1000.0,
            imu.acc.x, imu.acc.y, imu.acc.z,
            imu.omega.x, imu.omega.y, imu.omega.z,
            imu.temp);
            vTaskDelay(pdMS_TO_TICKS(100));

        #endif
    }

}