#include "mpu6050.h"
#include "i2c.h"
#include "systick.h"
#include "uart.h"

#define ACCEL_SCALE    16384
#define GYRO_SCALE     131
#define TILT_THRESHOLD 3000
#define FLAT_THRESHOLD 14000

void MPU6050_init(void) {
    I2C1_write_reg(MPU6050_ADDR, MPU_PWR_MGMT_1, 0x00);
    delay_ms(100);
}

uint8_t MPU6050_who_am_i(void) {
    return I2C1_read_reg(MPU6050_ADDR, MPU_WHO_AM_I);
}

MPU6050_Data_t MPU6050_read_accel(void) {
    MPU6050_Data_t accel;

    uint8_t xh = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H);
    uint8_t xl = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 1);
    uint8_t yh = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 2);
    uint8_t yl = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 3);
    uint8_t zh = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 4);
    uint8_t zl = I2C1_read_reg(MPU6050_ADDR, MPU_ACCEL_XOUT_H + 5);

    accel.x = (int16_t)((xh << 8) | xl);
    accel.y = (int16_t)((yh << 8) | yl);
    accel.z = (int16_t)((zh << 8) | zl);

    return accel;
}

MPU6050_Data_t MPU6050_read_gyro(void) {
    MPU6050_Data_t gyro;

    uint8_t xh = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H);
    uint8_t xl = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 1);
    uint8_t yh = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 2);
    uint8_t yl = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 3);
    uint8_t zh = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 4);
    uint8_t zl = I2C1_read_reg(MPU6050_ADDR, MPU_GYRO_XOUT_H + 5);

    gyro.x = (int16_t)((xh << 8) | xl);
    gyro.y = (int16_t)((yh << 8) | yl);
    gyro.z = (int16_t)((zh << 8) | zl);

    return gyro;
}

// helper - not exposed in header, only used internally
static void print_fixed(int16_t val, const char *unit) {
    UART2_print_int(val / 100);
    UART2_send_char('.');
    UART2_print_int(val % 100 < 0 ? -(val % 100) : val % 100);
    UART2_send_str(unit);
}

void MPU6050_print_raw(MPU6050_Data_t *accel, MPU6050_Data_t *gyro) {
    UART2_send_str("AX:"); UART2_print_int(accel->x);
    UART2_send_str(" AY:"); UART2_print_int(accel->y);
    UART2_send_str(" AZ:"); UART2_print_int(accel->z);
    UART2_send_str(" GX:"); UART2_print_int(gyro->x);
    UART2_send_str(" GY:"); UART2_print_int(gyro->y);
    UART2_send_str(" GZ:"); UART2_print_int(gyro->z);
    UART2_send_str("\r\n");
}

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

void MPU6050_print_direction(MPU6050_Data_t *accel) {
    UART2_send_str("X:");
    if(accel->x > TILT_THRESHOLD)       UART2_send_str("RIGHT  ");
    else if(accel->x < -TILT_THRESHOLD) UART2_send_str("LEFT   ");
    else                                 UART2_send_str("CENTER ");

    UART2_send_str(" Y:");
    if(accel->y > TILT_THRESHOLD)       UART2_send_str("FORWARD");
    else if(accel->y < -TILT_THRESHOLD) UART2_send_str("BACK   ");
    else                                 UART2_send_str("CENTER ");

    UART2_send_str(" Z:");
    if(accel->z > FLAT_THRESHOLD)       UART2_send_str("FLAT\r\n");
    else                                UART2_send_str("TILTED\r\n");
}
