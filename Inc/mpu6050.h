#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

// I2C address
#define MPU6050_ADDR     0x68

// Registers
#define MPU_PWR_MGMT_1   0x6B
#define MPU_WHO_AM_I     0x75
#define MPU_ACCEL_XOUT_H 0x3B
#define MPU_GYRO_XOUT_H  0x43

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} MPU6050_Data_t;

void MPU6050_init(void);
uint8_t MPU6050_who_am_i(void);
MPU6050_Data_t MPU6050_read_accel(void);
MPU6050_Data_t MPU6050_read_gyro(void);

void MPU6050_print_raw(MPU6050_Data_t *accel, MPU6050_Data_t *gyro);
void MPU6050_print_converted(MPU6050_Data_t *accel, MPU6050_Data_t *gyro);
void MPU6050_print_direction(MPU6050_Data_t *accel);


#endif
