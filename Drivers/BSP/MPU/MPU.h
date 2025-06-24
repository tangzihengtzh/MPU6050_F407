#include "i2c.h"
#include "gpio.h"
#include "fsmc.h"

// MPU6050 I2C 地址
#define MPU6050_ADDRESS        0x68

// MPU6050 寄存器地址
#define MPU6050_PWR_MGMT_1     0x6B
#define MPU6050_SMPLRT_DIV     0x19
#define MPU6050_CONFIG         0x1A
#define MPU6050_GYRO_CONFIG    0x1B
#define MPU6050_ACCEL_CONFIG   0x1C

// 函数声明
HAL_StatusTypeDef mpu_init(void);
HAL_StatusTypeDef MPU_Write_Register(uint8_t reg, uint8_t data);
HAL_StatusTypeDef MPU_Read_Register(uint8_t reg, uint8_t *data);

// 加速度计寄存器宏定义
#define ACCEL_XOUT_H           0x3B  // X 轴加速度高字节寄存器
#define ACCEL_XOUT_L           0x3C  // X 轴加速度低字节寄存器

#define ACCEL_YOUT_H           0x3D  // Y 轴加速度高字节寄存器
#define ACCEL_YOUT_L           0x3E  // Y 轴加速度低字节寄存器

#define ACCEL_ZOUT_H           0x3F  // Z 轴加速度高字节寄存器
#define ACCEL_ZOUT_L           0x40  // Z 轴加速度低字节寄存器

int16_t MPU_Read_Accel_X(void);

int16_t MPU_Read_Accel_Y(void);

int16_t MPU_Read_Accel_Z(void);
