/**
 * Infrared Proximity Sensors
 * this section contains low level code for servicing them
*/
#ifndef PROX_SENS
    #define PROX_SENS

    // includes for adcs
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "soc/soc_caps.h"
    #include "esp_log.h"
    #include "esp_adc/adc_oneshot.h"
    #include "esp_adc/adc_cali.h"
    #include "esp_adc/adc_cali_scheme.h"
    #include "driver/gpio.h"

    #include "main.h"

    //sensor struct
    typedef struct {
        const int index;
        const adc_channel_t channel;
        const gpio_num_t emitter_pin;
        const char* name;
    } prox_sensor_t;

    /*---------------------------------------------------------------
            ADC channel assignemnts
    ---------------------------------------------------------------*/

    #define V_SENSE           ADC_CHANNEL_0

    #define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_11
    
    #define LED_PIN           23

    extern prox_sensor_t sensors[4];

    void get_prox(prox_sensor_t sensor, int* out_raw, int* out_mv);

    extern int adc_raw[5];
    extern int voltage[5];

    bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle);

    void adc_calibration_deinit(adc_cali_handle_t handle);

    extern adc_oneshot_unit_handle_t adc2_handle;
    extern adc_cali_handle_t adc2_cali_handle;

    void sensor_init(void);
#endif