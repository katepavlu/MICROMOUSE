#include "./adc.h"


int adc_raw[5];
int voltage[5];

//-------------ADC2 Config---------------//
adc_oneshot_unit_handle_t adc2_handle;
adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = EXAMPLE_ADC_ATTEN,
};
adc_oneshot_unit_init_cfg_t init_config2 = {
    .unit_id = ADC_UNIT_2,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
};
adc_cali_handle_t adc2_cali_handle = NULL;

prox_sensor_t sensors[]={
    {
    .index = 0,
    .channel = ADC_CHANNEL_5,
    .emitter_pin = 27,
    .name = "L"
    },

    {
    .index = 1,
    .channel = ADC_CHANNEL_3,
    .emitter_pin = 14,
    .name = "FL"
    },
    
    {
    .index = 2,
    .channel = ADC_CHANNEL_2,
    .emitter_pin = 16,
    .name = "FR"
    },

    {
    .index = 3,
    .channel = ADC_CHANNEL_4,
    .emitter_pin = 17,
    .name = "R"
    }
};

//---------Sensor GPIO config------------//
//zero-initialize the config structure.
gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_DISABLE, //disable interrupt
    .mode = GPIO_MODE_OUTPUT, //set as output mode
    .pull_down_en = 0, //disable pull-down mode
    .pull_up_en = 0 //disable pull-up mode
};

void sensor_init(void){

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config2, &adc2_handle)); //initialise the adc
    adc_calibration_init(ADC_UNIT_2, EXAMPLE_ADC_ATTEN, &adc2_cali_handle); //initialise the calibration
    for(int i=0; i<4; i++){
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, sensors[i].channel, &config)); // configure oneshot adcs for the sensors
    }

    for(int i=0; i<4; i++){
        io_conf.pin_bit_mask |= 1ULL<<sensors[i].emitter_pin; // add sensor emitter pins as output GPIO
    }

    gpio_config(&io_conf); //configure GPIO with the given settings

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, V_SENSE, &config));
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle)
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

void adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}

void get_prox(prox_sensor_t sensor, int* out_raw, int* out_mv)
{
    gpio_set_level(sensor.emitter_pin, 1);  // light up the sensor LED
    vTaskDelay(pdMS_TO_TICKS(3));           // give it time to stabilise

    // measure voltage at the sensor
    int sum = 0;
    for(int i=0; i<10; i++){
        ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, sensor.channel, &out_raw[sensor.index]));
        sum += out_raw[sensor.index]; //averaging from 10 consecutive values
    }
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc2_cali_handle, sum/10, &out_mv[sensor.index]));
    ESP_LOGI(TAG, "Sensor %s Voltage: %d mV", sensor.name, out_mv[sensor.index]);

    gpio_set_level(sensor.emitter_pin, 1);  // turn off the sensor LED
    vTaskDelay(pdMS_TO_TICKS(5));           // give it enough time to turn off completely
}