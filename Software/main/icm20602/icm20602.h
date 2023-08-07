#ifndef ICM20602
    #define ICM20602

    #include <stdio.h>
    #include "esp_log.h"
    #include "driver/i2c.h"

    #include "main.h"
    #include "ICM20602_registers.h"

    #define I2C_MASTER_SCL_IO           22      /*!< GPIO number used for I2C master clock */
    #define I2C_MASTER_SDA_IO           21      /*!< GPIO number used for I2C master data  */
    #define I2C_MASTER_NUM              0       /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
    #define I2C_MASTER_FREQ_HZ          400000  /*!< I2C master clock frequency */
    #define I2C_MASTER_TX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
    #define I2C_MASTER_RX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
    #define I2C_MASTER_TIMEOUT_MS       1000

    #define ICM20602_SENSOR_ADDR        0x68    /*!< Slave address of the IMU */

    #define GYRO_SENSITIVITY 250.0 // gyro full scale +- 250 deg/s
    #define ACC_SENSITIVITY 2*gravity // accelometer full scale +- 2g

    typedef struct{
        double x;
        double y;
        double z;
    }vector_t;

    typedef struct{
        vector_t acc;
        vector_t omega;
        double temp;
    }imu_t;

    extern esp_err_t ICM20602_register_read(uint8_t reg_addr, uint8_t *data, size_t len);

    extern esp_err_t ICM20602_register_write_byte(uint8_t reg_addr, uint8_t data);

    extern esp_err_t i2c_master_init(void);

    float convert_temp(int temp_raw);

    float convert_acc(int acc_raw);

    float convert_gyro(int gyro_raw);

    imu_t* imu_read(imu_t* imu);


    void imu_init(void);

    void imu_sleep(void);

    void imu_deinit(void);

    void imu_wake(void);

#endif