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



#include "batt_soc/batt_soc.h"
#


const static char *TAG = "MICROMOUSE";

typedef struct {
    int index;
    adc_channel_t channel;
    adc_oneshot_unit_handle_t handle;
    adc_cali_handle_t cali_handle;
    gpio_num_t emitter_pin;
    char* name;
} prox_sensor_t;

/*---------------------------------------------------------------
        ADC General Macros
---------------------------------------------------------------*/
//we're only using ADC2
#define V_SENSE          ADC_CHANNEL_0
#define SENSOR_OUT_L     ADC_CHANNEL_5
#define SENSOR_OUT_FL    ADC_CHANNEL_3
#define SENSOR_OUT_FR    ADC_CHANNEL_2
#define SENSOR_OUT_R     ADC_CHANNEL_4

#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_11

static int adc_raw[5];
static int voltage[5];
static bool example_adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void example_adc_calibration_deinit(adc_cali_handle_t handle);
bool get_prox(prox_sensor_t sensor, int* out_raw, int* out_mv);

#define SENSOR_IN_L     27
#define SENSOR_IN_FL    14
#define SENSOR_IN_FR    16
#define SENSOR_IN_R     17
#define LED_PIN         23

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<SENSOR_IN_L) | (1ULL<<SENSOR_IN_FL) | (1ULL<<SENSOR_IN_FR) | (1ULL<<SENSOR_IN_R) | (1ULL<<LED_PIN))

void app_main(void)
{
    //---------Sensor GPIO config------------//
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //-------------ADC2 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = EXAMPLE_ADC_ATTEN,
    };

    //-------------ADC2 Init---------------//
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t init_config2 = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config2, &adc2_handle));

    //-------------ADC2 Calibration Init---------------//
    adc_cali_handle_t adc2_cali_handle = NULL;
    bool do_calibration2 = example_adc_calibration_init(ADC_UNIT_2, EXAMPLE_ADC_ATTEN, &adc2_cali_handle);

    //-------------ADC2 Config---------------//
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, V_SENSE, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, SENSOR_OUT_L, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, SENSOR_OUT_FL, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, SENSOR_OUT_FR, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, SENSOR_OUT_R, &config));


    prox_sensor_t sensors[4];
    
    sensors[0] = (prox_sensor_t){
        .index = 0,
        .channel = SENSOR_OUT_L,
        .handle = adc2_handle,
        .cali_handle = adc2_cali_handle,
        .emitter_pin = SENSOR_IN_L,
        .name = "L" };
    
    sensors[1] = (prox_sensor_t){
        .index = 1,
        .channel = SENSOR_OUT_FL,
        .handle = adc2_handle,
        .cali_handle = adc2_cali_handle,
        .emitter_pin = SENSOR_IN_FL,
        .name = "FL" };
    
    sensors[2] = (prox_sensor_t){
        .index = 2,
        .channel = SENSOR_OUT_FR,
        .handle = adc2_handle,
        .cali_handle = adc2_cali_handle,
        .emitter_pin = SENSOR_IN_FR,
        .name = "FR" };

    sensors[3] = (prox_sensor_t){
        .index = 3,
        .channel = SENSOR_OUT_R,
        .handle = adc2_handle,
        .cali_handle = adc2_cali_handle,
        .emitter_pin = SENSOR_IN_R,
        .name = "R" };

   

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

    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc2_handle));
    if (do_calibration2) {
        example_adc_calibration_deinit(adc2_cali_handle);
    }
}


bool get_prox(prox_sensor_t sensor, int* out_raw, int* out_mv)
{
    gpio_set_level(sensor.emitter_pin, 1);  // light up the sensor LED
    vTaskDelay(pdMS_TO_TICKS(3));           // give it time to stabilise

    // measure voltage at the sensor
    int sum = 0;
    for(int i=0; i<10; i++){
        ESP_ERROR_CHECK(adc_oneshot_read(sensor.handle, sensor.channel, &out_raw[sensor.index]));
        sum += out_raw[sensor.index]; //averaging from 10 consecutive values
    }
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(sensor.cali_handle, sum/10, &out_mv[sensor.index]));
    ESP_LOGI(TAG, "Sensor %s Voltage: %d mV", sensor.name, out_mv[sensor.index]);

    gpio_set_level(sensor.emitter_pin, 1);  // turn off the sensor LED
    vTaskDelay(pdMS_TO_TICKS(5));           // give it enough time to turn off completely
    return 1;
}


/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool example_adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

static void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}