#include "./icm20602.h"

uint8_t data[2];

/**
 * @brief Read a sequence of bytes from a ICM20602 sensor registers
 */
esp_err_t ICM20602_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, ICM20602_SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief Write a byte to a ICM20602 sensor register
 */
esp_err_t ICM20602_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, ICM20602_SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

float convert_temp(int temp_raw)
{
    return (float)temp_raw / 326.8 + 25.0;
}

float convert_acc(int acc_raw)
{
    return (float)acc_raw * ACC_SENSITIVITY / max_signed_16;
}

float convert_gyro(int gyro_raw)
{
    return (float)gyro_raw * GYRO_SENSITIVITY / max_signed_16 * pi / 180.0;
}

imu_t* imu_read(imu_t* imu)
{
    int16_t gyro[3];
    int16_t accel[3];
    int16_t temp;

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_ACCEL_XOUT_H, data, 2));
    accel[0] = (data[0]<<8) + data[1];

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_ACCEL_YOUT_H, data, 2));
    accel[1] = (data[0]<<8) + data[1];

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_ACCEL_ZOUT_H, data, 2));
    accel[2] = (data[0]<<8) + data[1];

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_GYRO_XOUT_H, data, 2));
    gyro[0] = (data[0]<<8) + data[1];

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_GYRO_YOUT_H, data, 2));
    gyro[1] = (data[0]<<8) + data[1];

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_GYRO_ZOUT_H, data, 2));
    gyro[2] = (data[0]<<8) + data[1];

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_TEMP_OUT_H, data, 2));
    temp = (data[0]<<8) + data[1];

    imu->acc.x = convert_acc(accel[0]);
    imu->acc.y = convert_acc(accel[1]);
    imu->acc.z = convert_acc(accel[2]);

    imu->omega.x = convert_acc(gyro[0]);
    imu->omega.y = convert_acc(gyro[1]);
    imu->omega.z = convert_acc(gyro[2]);

    imu->temp = convert_temp(temp);

    ESP_LOGI(TAG, "Accelerometer: X%.3f Y%.3f Z%.3f, Gyro: X%.3f Y%.3f Z%.3f, Temp: %.1f C", 
    imu->acc.x, imu->acc.y, imu->acc.z, imu->omega.x, imu->omega.y, imu->omega.z, imu->temp);

    return imu;
}


void imu_init(void)
{
    char buffer[17];
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    imu_wake();

    /* Read the ICM20602 WHO_AM_I register, on power up the register should have the value 0x12 */
    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_WHO_AM_I, data, 1));
    ESP_LOGI(TAG, "WHO_AM_I = %X", data[0]);

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_CONFIG, data, 1));
    __itoa(data[0], buffer, 2);
    ESP_LOGI(TAG, "CONFIG register = %s", buffer);

    ESP_ERROR_CHECK(ICM20602_register_read(ICM20602_PWR_MGMT_1, data, 1));
    __itoa(data[0], buffer, 2);
    ESP_LOGI(TAG, "Power management register = %s", buffer);

}

void imu_sleep(void)
{
    ESP_ERROR_CHECK(ICM20602_register_write_byte(ICM20602_PWR_MGMT_1, 1<<7 | 0x001));
    ESP_LOGI(TAG, "Sensor put to sleep");
}

void imu_deinit(void)
{
    imu_sleep();
    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C de-initialized successfully");
}

void imu_wake(void)
{
    ESP_ERROR_CHECK(ICM20602_register_write_byte(ICM20602_PWR_MGMT_1, 0x001));
    ESP_LOGI(TAG, "Sensor woken from sleep");
}