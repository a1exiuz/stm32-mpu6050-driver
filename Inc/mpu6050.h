/*
    mpu6050.h
    Driver for InvenSense MPU-6050 6-axis IMU (accelerometer + gyroscopte)
    Interface: I2C1 at 100kHz, device address 0x68 (AD0 tied to GND)
    Provides raw read, unit-converted, and direction output via UART
    Reference: MPU-6050 Product Specification, Register Map and Descriptions
*/
#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

// I2C address - AD0 pin tied to GND sets address to 0x68
#define MPU6050_ADDR     0x68

// Registers addresses
#define MPU_PWR_MGMT_1   0x6B // power management - bit 6 = sleep mode
#define MPU_WHO_AM_I     0x75 // device ID register - always retuurns 0x68
#define MPU_ACCEL_XOUT_H 0x3B // accel X high byte - floowed by XL, YH, YL, ZH, ZL
#define MPU_GYRO_XOUT_H  0x43 // gyro X high byte - followed by XL, Yh, YL, ZH, ZL

/*
    MPU6050_Data_t
    Holds raw 16-bit signed values for one sensor axis set (accel or gyro)
    Accel scale: 16384 LSB/g at defualt ±2g range
    Gyro Scale: 131 LSB/ºs at defualt ±250º/s range
*/
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} MPU6050_Data_t;

/*
    MPU6050_init
    Wakes MPU6050 from sleep by clearing PWR_MGMT_1 register
    Must be called after I2C1_init and WHO_AM_I verification
    Delays 100ms after wake to allow sensor to stabilize
*/
void MPU6050_init(void);

/*
    MPU6050_who_am_i
    Reads the WHO_AM_I register
    Returns 0x68 if sensor is connected and responding correctly
    Use to verify I2C communication before reading sensor data
*/
uint8_t MPU6050_who_am_i(void);

/*
    MPU6050_read_accel
    Reads raw accelerometer X, Y, Z from registers 0x3B-0x40
    Each axis is 16 bit signed, high byte first
    Returns MPU6050_Data_t with raw counts
    Divide by 16384 to conver to g
*/
MPU6050_Data_t MPU6050_read_accel(void);

/*
    MPU6050_read_gyro
    Reads raw gyroscope X, Y, Z from registesr 0x43-0x48
    Each axis is 16-bit signed, high byte first
    Returns MPU6050_Data_t with raw counts
    Divide by 131 to conver to degrees per second
*/
MPU6050_Data_t MPU6050_read_gyro(void);

/*
    MPU6050_print_raw
    Prints raw accel and gyro counts over UART2
    Format: "AX:1234 AY:-567 AZ: 16200 GX:12 GY:-8 GZ:3"
*/
void MPU6050_print_raw(MPU6050_Data_t *accel, MPU6050_Data_t *gyro);

/*
    MPU6050_print_converted
    Prints accel in g and gyro in degrees/second over UART2
    Format: "AX:0.07g AY:-0.03g AZ:0.98g GX:0.09d GY=-0.06d GZ:0.02d"
    Uses integer math - no floating point
*/
void MPU6050_print_converted(MPU6050_Data_t *accel, MPU6050_Data_t *gyro);

/*
    MPU6050_print_direction
    Prints tilt direction based on accel thresholds over UART2
    Format: "X:LEFT Y:FORWARED Z: TILTED"
    Treshold: 3000 raw counts (~0.18g) to register a direction
    Z axis: >14000 counts = FLAT, else TILTED
*/
void MPU6050_print_direction(MPU6050_Data_t *accel);


#endif
