/**
* @file mpu6050.c
*
* @brief Driver for MPU6050 6-axis IMU sensor
*
* Provides initialization, raw sensor reads, and formatted UART output
* for the MPU-6050 accelerometer and gyroscope over I2C1
*
* Three output modes are supported:
*    - RAW: raw 16-bit signed counts direct from registers 
*    - CONVERTED: scaled to g (accel) and degrees/s (gyro)
*    - DIRECTION: tilt direction based on thresholds
*
* Scaling factors at defuault full-scale range:
*    - Accelerometer: 16384 LSB/g at +/- 2g
*    - Gyroscope: 131 LSB/(degrees/s) at +/- 250 degrees/s
*
* @note I2C1 must be initialized via I2C1_init() before calling
*      any function in this driver 
*
* @ref MPU-6050 Prouct Specification Rev 3.4
* @ref MPU-6050 Register Map and Descriptiosn Rev 4.2
*/

#include "mpu6050.h"
#include "i2c.h"
#include "systick.h"
#include "uart.h"

#define ACCEL_SCALE    16384    /* LSB/g  at ±2g range  */
#define GYRO_SCALE      131     /* LSB/°s at ±250°/s range */
#define TILT_THRESHOLD  3000    /* ~0.18g - minimum count to register a tilt */
#define FLAT_THRESHOLD  14000   /* ~0.85g - threshold for Z axis flat detection */

/** 
* @brief Wakes the MPU6050 and allows sensor to stabilize
*
* Clears the SLEEP bit in PWR_MGMT_1 register (0x6B) to wake the device
* Delays 100ms after wake to allow internal oscillator to stabilize
*
* @note Must be called after I2C1_init(). Verify sensor is present
*       with MPU6050_who_am_i() before calling this function.
*
* @retval None
*/
void MPU6050_init(void) {
    I2C1_write_reg(MPU6050_ADDR, MPU_PWR_MGMT_1, 0x00); /* clears sleep bit */
    delay_ms(100); /* delay for oscillator stabilization */
}

/**
* @brief Reads the WHO_AM_I register to verify sensor presence
*
* @retval 0x68 if MPU6050 is connected and responding correctly
* @retval othe value indicates wrong device or communication failure
*/
uint8_t MPU6050_who_am_i(void) {
    return I2C1_read_reg(MPU6050_ADDR, MPU_WHO_AM_I);
}

/**
* @brief Reads raw accelerometer X, Y, Z axes from the MPU6050
*
* Reads 6 consecutive registers starting at MPU_ACCEL_XOUT_H (0x38)
* Each axis is 16-bit signed, transmitted high byte first 
*
* @note Divide raw counts by ACCEL_SCALE (16384) to convert to g
*
* @retval MPU6050_Data_t struct containing raw X, Y, Z accelerometer counts  
*/
MPU6050_Data_t MPU6050_read_accel(void) {
    MPU6050_Data_t accel;

    uint8_t xh = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H);
    uint8_t xl = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 1);
    uint8_t yh = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 2);
    uint8_t yl = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 3);
    uint8_t zh = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 4);
    uint8_t zl = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 5);

    accel.x = (int16_t)((xh << 8) | xl); /* combine high and low bytes */
    accel.y = (int16_t)((yh << 8) | yl);
    accel.z = (int16_t)((zh << 8) | zl);

    return accel;
}

/**
*@brief Reads raw gyroscope X, Y, Z axes from the MPU6050
*
* Reads 6 consecutive registers starting at MPU_GYRO_XOUT_H (0x43)
* Each axis is 16-bit signed, transmitted high byte first
*
*@note Divide raw counts by GYRO_SCALE (131) to convert to degrees/s
*
*@retval MPU6050_Data_t struct containing raw X, Y, Z gyroscope counts
*/
MPU6050_Data_t MPU6050_read_gyro(void) {
    MPU6050_Data_t gyro;

    uint8_t xh = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H);
    uint8_t xl = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 1);
    uint8_t yh = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 2);
    uint8_t yl = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 3);
    uint8_t zh = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 4);
    uint8_t zl = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 5);

    gyro.x = (int16_t)((xh << 8) | xl); /* combine high and low bytes */
    gyro.y = (int16_t)((yh << 8) | yl);
    gyro.z = (int16_t)((zh << 8) | zl);

    return gyro;
}

/**
*@brief Prints a fixed-point value with two decimal palces over UART
*
* Internal Helper - not exposed in the public header
* Value is expected pre-sclaed by 100 (e.g. 98 prints as "0.98")
*
*@param val Pre-scaled integer value (actual value * 100)
*@param uint unit string to append (e.g. "g ", "d ")
*
*@retval None
*/
static void print_fixed(int16_t val, const char *unit) {
    UART2_print_int(val / 100);
    UART2_send_char('.');
    UART2_print_int(val % 100 < 0 ? -(val % 100) : val % 100);
    UART2_send_str(unit);
}

/**
* @brief Prints raw accelerometer and gyroscope data over UART
*
* Format: "AX:123 AY:-456 AZ:789 GX:12 GY:-34 GZ:56"
*
* @param accel Pointer to MPU6050_Data_t containing raw accelerometer data
* @param gyro Pointer to MPU6050_Data_t containing raw gyroscope data
*
* @retval None 
*/
void MPU6050_print_raw(MPU6050_Data_t *accel, MPU6050_Data_t *gyro) {
    UART2_send_str("AX:"); UART2_print_int(accel->x);
    UART2_send_str(" AY:"); UART2_print_int(accel->y);
    UART2_send_str(" AZ:"); UART2_print_int(accel->z);
    UART2_send_str(" GX:"); UART2_print_int(gyro->x);
    UART2_send_str(" GY:"); UART2_print_int(gyro->y);
    UART2_send_str(" GZ:"); UART2_print_int(gyro->z);
    UART2_send_str("\r\n");
}

/**
* @brief Prints converted accelerometer and gyroscope data over UART
*
* Uses integer arithmetic scaled by 100 for two decimal places without 
* floating point. Format: "AX:0.98g AY:-0.45g AZ:1.02g GX:12.34d GY:-5.67d GZ:0.89d"
*
* @param accel Pointer to MPU6050_Data_t containing raw accelerometer data
* @param gyro Pointer to MPU6050_Data_t containing raw gyroscope data
*
* @note Uses integer arithmetic only - no floating point required
*
* @retval None
*/
void MPU6050_print_converted(MPU6050_Data_t *accel, MPU6050_Data_t *gyro) {
    int16_t ax_g = (int16_t)(((int32_t)accel->x * 100) / ACCEL_SCALE);
    int16_t ay_g = (int16_t)(((int32_t)accel->y * 100) / ACCEL_SCALE);
    int16_t az_g = (int16_t)(((int32_t)accel->z * 100) / ACCEL_SCALE);
    int16_t gx_d = (int16_t)(((int32_t)gyro->x  * 100) / GYRO_SCALE);
    int16_t gy_d = (int16_t)(((int32_t)gyro->y  * 100) / GYRO_SCALE);
    int16_t gz_d = (int16_t)(((int32_t)gyro->z  * 100) / GYRO_SCALE);

    UART2_send_str("AX:"); print_fixed(ax_g, "g ");
    UART2_send_str("AY:"); print_fixed(ay_g, "g ");
    UART2_send_str("AZ:"); print_fixed(az_g, "g ");
    UART2_send_str("GX:"); print_fixed(gx_d, "d ");
    UART2_send_str("GY:"); print_fixed(gy_d, "d ");
    UART2_send_str("GZ:"); print_fixed(gz_d, "d\r\n");
}

/**
* @brief Prints tilt direction based on accelerometer thresholds over UART
*
* Compares raw accelerometer counts againt TILT_THRESHOLD and FLAT_THRESHOLD
* to determine orientation. FORMAT: "X:RIGHT Y:FORWARD Z:FLAT"
*
* @param accel Pointer to MPU6050_Data_t containing raw accelerometer data
*
* @note Thresholds are define as:
*       - TilT_THRESHOLD 3000 counts (~0.18g) to register X/Y tilt
*       - FLAT_THRESHOLD 14000 counts (~0.85g) for Z axis flat detection
*
* @retval None
*/
void MPU6050_print_direction(MPU6050_Data_t *accel) {
    UART2_send_str("X:");
    if(accel->x > TILT_THRESHOLD)       UART2_send_str("LEFT  ");
    else if(accel->x < -TILT_THRESHOLD) UART2_send_str("RIGHT   ");
    else                                 UART2_send_str("CENTER ");

    UART2_send_str(" Y:");
    if(accel->y > TILT_THRESHOLD)       UART2_send_str("FORWARD");
    else if(accel->y < -TILT_THRESHOLD) UART2_send_str("BACK   ");
    else                                 UART2_send_str("CENTER ");

    UART2_send_str(" Z:");
    if(accel->z > FLAT_THRESHOLD)       UART2_send_str("FLAT\r\n");
    else                                UART2_send_str("TILTED\r\n");
}
