/*
ICM20602 IMU register definitions
i2c: max 400kHz

adress: b110100X where X is the logical level at pin SA0

self test enable registers: 27 and 28
self test evaluation:
sensor data with self test enabled - sensor data with it disabled
needs to fall between some value -
accelerometer:
groscope:


reset values are 0x00 for everything except CONFIG, PWR_MGMT_1, WHO_AM_I and self test registers

detailed descriptions in https://invensense.tdk.com/wp-content/uploads/2016/10/DS-000176-ICM-20602-v1.0.pdf
(section 9, p32)
*/
#ifndef ICM20602_REGISTERS

    #define ICM20602_REGISTERS

    #define ICM20602_XG_OFFS_TC_H 0x04
    #define ICM20602_XG_OFFS_TC_L 0x05
    #define ICM20602_YG_OFFS_TC_H 0x07
    #define ICM20602_YG_OFFS_TC_L 0x08
    #define ICM20602_ZG_OFFS_TC_H 0x0A
    #define ICM20602_ZG_OFFS_TC_L 0x0B
    /*
    ICM20602_*G_OFFS_TC_H: bits 7-2 = *G_OFFS_LP, bits 1-0 = *G_OFFS_TC[9:8]
    ICM20602_*G_OFFS_TC_H: *G_OFFS_TC[7:0]

    *G_OFFS_LP is a 6b offset between low noise and low power modes, needs to be applied in software
    *G_OFFS_TC is an automatically applied temperature correction
    */

    #define ICM20602_SELF_TEST_X_ACCEL 0x0D
    #define ICM20602_SELF_TEST_Y_ACCEL 0x0E
    #define ICM20602_SELF_TEST_Z_ACCEL 0x0F
    /* 
    factory self test results, for reference against user performed self tests
    */

    #define ICM20602_XG_OFFS_USRH 0x13
    #define ICM20602_XG_OFFS_USRL 0x14
    #define ICM20602_YG_OFFS_USRH 0x15
    #define ICM20602_YG_OFFS_USRL 0x16
    #define ICM20602_ZG_OFFS_USRH 0x17
    #define ICM20602_ZG_OFFS_USRL 0x18
    /*
    automatically applied DC offsets for the gyro
    */

    #define ICM20602_SMPLRT_DIV    0x19 //sample rate divider - sample rate = (internal rate)/(divideer + 1)
    #define ICM20602_CONFIG        0x1A
    #define ICM20602_GYRO_CONFIG   0x1B
    #define ICM20602_ACCEL_CONFIG  0x1C
    #define ICM20602_ACCEL_CONFIG2 0x1D
    #define ICM20602_LP_MODE_CFG   0x1E
    /*
    CONFIG: bit 6 = FIFO_MODE, bits 5-3 = EXT_SYNC_SET, bits 2-0 = DLPF_CFG
    GYRO_CONFIG: bits 7-5 = [X,Y,Z]G_ST, bits 4-3 = FS_SEL, bits 2-0 =  FCHOICE_B
    ACCEL_CONFIG: bits 7-5 = [X,Y,Z]A_ST, bits 4-3 = ACCEL_FS_SEL
    ACCEL_CONFIG2: bits 5-4 = DEC2_CFG, bit 4 = ACCEL_FCHOICE_B, bits 2-0 = A_DLPF_CFG
    LP_MODE_CFG: bit 7 = GYRO_CYCLE, bits 6-4 = G_AVGCFG
    */

    #define ICM20602_ACCEL_WOM_X_THR 0x20
    #define ICM20602_ACCEL_WOM_Y_THR 0x21
    #define ICM20602_ACCEL_WOM_Z_THR 0x22

    #define ICM20602_FIFO_EN 0x23
    /*
    FIF_EN: bit 4 = GYRO_FIFO_EN, bit 3 = ACCEL_FIFO_EN
    */

    #define ICM20602_FSYNC_INT          0x36
    #define ICM20602_INT_PIN_CFG        0x37
    #define ICM20602_INT_ENABLE         0x38
    #define ICM20602_FIFO_WM_INT_STATUS 0x39
    #define ICM20602_INT_STATUS 0x3A
    /*
    FSYNC_INT: bit 7 = FSYNC_INT
    INT_PIN_CFG: bit 7 = INT_LEVEL, bit 6 = INT_OPEN, bit 5 = LATCH_INT_EN,
     bit 4 = INT_RD_CLEAR, bit 3 = FSYNC_INT_LEVEL, bit 2 = FSYNC_INT_MODE_EN
    INT_ENABLE: bits 7-5 = WOM_[X,Y,Z]_INT_EN, bit 4 = FIFO_OFLOW_EN,
     bit 3 = FSYNC_INT_EN, bit 2 = GDRIVE_INT_EN, bit 0 = DATA_RDY_INT_EN
    (read 2 clear)FIFO_WM_INT_STATUS: bit 6 = FIFO_WM_INT
    (read 2 clear)INT_STATUS: bits 7-5 = WOM_[X,Y,Z]_INT, bit 4 = FIFO_OFLOW_INT,
     bit 2 = GDRIVE_INT, bit 0 = DATA_RDY_INT
    */

    #define ICM20602_ACCEL_XOUT_H 0x3B
    #define ICM20602_ACCEL_XOUT_L 0x3C
    #define ICM20602_ACCEL_YOUT_H 0x3D
    #define ICM20602_ACCEL_YOUT_L 0x3E
    #define ICM20602_ACCEL_ZOUT_H 0x3F
    #define ICM20602_ACCEL_ZOUT_L 0x40

    #define ICM20602_TEMP_OUT_H 0x41
    #define ICM20602_TEMP_OUT_L 0x42

    #define ICM20602_GYRO_XOUT_H 0x43
    #define ICM20602_GYRO_XOUT_L 0x44
    #define ICM20602_GYRO_YOUT_H 0x45
    #define ICM20602_GYRO_YOUT_L 0x46
    #define ICM20602_GYRO_ZOUT_H 0x47
    #define ICM20602_GYRO_ZOUT_L 0x48

    #define ICM20602_SELF_TEST_X_GYRO 0x50
    #define ICM20602_SELF_TEST_Y_GYRO 0x51
    #define ICM20602_SELF_TEST_Z_GYRO 0x52

    #define ICM20602_FIFO_WM_TH1 0x60
    #define ICM20602_FIFO_WM_TH2 0x61
    /*
    FIFO_WM_TH1: bits 1-0 = FIFO_WM_TH[9:8]
    FIFO_WM_TH1: FIFO_WM_TH[7:0]
    */

    #define ICM20602_SIGNAL_PATH_RESET 0x68
    #define ICM20602_ACCEL_INTEL_CTRL  0x69
    #define ICM20602_USER_CTRL         0x6A
    #define ICM20602_PWR_MGMT_1        0x6B
    #define ICM20602_PWR_MGMT_2        0x6C
    /*
    SIGNAL_PATH_RESET: bit 1 = ACCEL_RST, bit 0 = TEMP_RST
    ACCEL_INTEL_CTRL: bit 7 = ACCEL_INTEL_EN, bit 6 = ACCEL_INTEL_MODE, bit 1 = OUTPUT_LIMIT, bit 0 = WOM_TH_MODE
    USER_CTRL: bit 6 = FIFO_EN, bit 2 = FIFO_RST, bit 0 = SIG_COND_RST
    PWR_MGMT_1: bit 7 = DEVICE_RESET, bit 6 = SLEEP, bit % = CYCLE, bit % = GYRO STANDBY, bit 3 = TEMP_DIS, bits 2-0 = CLKSEL
    PWR_MGMT_2: bits 5-3 = STBY_[X,Y,Z]A, bits 2-0 = STBY_[X,Y,Z]G
    */

    #define ICM20602_I2C_IF 0x70
    /*
    I2C_IF: bit 6 = I2C_IF_DIS
    */

    #define ICM20602_FIFO_COUNTH 0x72
    #define ICM20602_FIFO_COUNTL 0x73
    #define ICM20602_FIFO_R_W    0x74
    #define ICM20602_WHO_AM_I    0x75 // devicce ID = 0x12

    #define ICM20602_XA_OFFSET_H 0x77 // 15b values
    #define ICM20602_XA_OFFSET_L 0x78 // only first 7b

    #define ICM20602_YA_OFFSET_H 0x7A
    #define ICM20602_YA_OFFSET_L 0x7B // only first 7b

    #define ICM20602_ZA_OFFSET_H 0x7D
    #define ICM20602_ZA_OFFSET_L 0x7E // only first 7b
#endif