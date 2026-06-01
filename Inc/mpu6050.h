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
#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

/**
* @defgroup MPU6050_ADDR MPU6050 I2C Address
* @{
*/
#define MPU6050_ADDR     0x68 /**< I2C address - ADO pin tied to GND */
/** @} */

#define MPU_PWR_MGMT_1   0x6B  /**< Power management — bit 6 = sleep mode */
#define MPU_WHO_AM_I     0x75  /**< Device ID — always returns 0x68 */
#define MPU_ACCEL_XOUT_H 0x3B  /**< Accel X high byte — followed by XL,YH,YL,ZH,ZL */
#define MPU_GYRO_XOUT_H  0x43  /**< Gyro X high byte  — followed by XL,YH,YL,ZH,ZL */
/** @} */

/**
* @brief Raw sensor data for one axis set (accelerometer or gyroscope)
* 
* Values are 16-bit signed integers direct from the sensor registers.
* Convert to physical units by dividing by the appropirate scale factor:
*       - Accel: divide by 16384 to get g
*       - Gyro: divide by 131 to get degrees/second
*/
typedef struct {
    int16_t x;  /**< X axis raw count */
    int16_t y;  /**< Y axis raw count */
    int16_t z;  /**< Z axis raw count */
} MPU6050_Data_t;

/**
* @brief Wakes the MPU6050 from sleep and allows sensor to stabilize.
*
* Clears the SLEEP bit in PWR_MGMT_1 (0x6B) then delays 100ms.
*
* @note Must be called after I2C1_init(). Verify sensor is present
*       with MPU6050_who_am_i() before calling this function.
*
* @retval  None
*/
void MPU6050_init(void);

/**
 * @brief Reads the WHO_AM_I register to verify sensor presence
 *
 * @retval 0x68 if MPU6050 is connected and responding correctly
 * @retval other value indicates wrong device or communication failure
 */
uint8_t MPU6050_who_am_i(void);

/**
 * @brief Reads raw accelerometer X, Y, Z axes from the MPU6050.
 *
 * Reads 6 consecutive registers starting at MPU_ACCEL_XOUT_H (0x3B).
 * Each axis is 16-bit signed, transmitted high byte first.
 *
 * @note Divide raw counts by ACCEL_SCALE (16384) to convert to g.
 *
 * @retval MPU6050_Data_t struct containing aw X, Y, Z accelerometer counts
 */
MPU6050_Data_t MPU6050_read_accel(void);

/**
 * @brief Reads raw gyroscope X, Y, Z axes from the MPU6050.
 *
 * Reads 6 consecutive registers starting at MPU_GYRO_XOUT_H (0x43).
 * Each axis is 16-bit signed, transmitted high byte first.
 *
 * @note Divide raw counts by GYRO_SCALE (131) to convert to degrees/s
 *
 * @retval MPU6050_Data_t struct containing raw X, Y, Z gyroscope counts
 */
MPU6050_Data_t MPU6050_read_gyro(void);

/**
 * @brief Prints raw accelerometer and gyroscope data over UART
 *
 * Format: "AX:123 AY:-456 AZ:789 GX:12 GY:-34 GZ:56"
 *
 * @param accel Pointer to MPU6050_Data_t containing raw accelerometer data
 * @param gyro Pointer to MPU6050_Data_t containing raw gyroscope data
 *
 * @retval  None
 */
void MPU6050_print_raw(MPU6050_Data_t *accel, MPU6050_Data_t *gyro);

/**
 * @brief Prints converted accelerometer and gyroscope over UART
 *
 * Uses integer arithmetic scaled by 100 for two decimal places without
 * floating point. Format: "AX:0.07g AY:-0.03g AZ:0.98g GX:0.09d GY:-0.06d GZ:0.02d"
 *
 * @param accel Pointer to MPU6050_Data_t containing raw accelerometer data
 * @param gyro Pointer to MPU6050_Data_t containing raw gyroscope data
 *
 * @note Uses interger arithmetic only - no floating point required
 *
 * @retval None
 */
void MPU6050_print_converted(MPU6050_Data_t *accel, MPU6050_Data_t *gyro);

/**
 * @brief   Prints tilt direction based on accelerometer thresholds over UART
 *
 * Format: "X:LEFT  Y:FORWARD  Z:FLAT"
 *
 * Thresholds applied to raw counts:
 *   - X/Y : 3000 counts (~0.18g) to register LEFT/RIGHT/FORWARD/BACK
 *   - Z   : 14000 counts (~0.85g) for FLAT, else TILTED
 *
 * @param accel Pointer to MPU6050_Data_t containing accelerometer data
 *
 * @retval None
 */
void MPU6050_print_direction(MPU6050_Data_t *accel);


#endif
