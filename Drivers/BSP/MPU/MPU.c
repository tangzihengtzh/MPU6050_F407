#include "MPU.h"


//#define MPU6050_ADDRESS        0x68  // MPU6050 的 I2C 地址
//#define MPU6050_PWR_MGMT_1     0x6B  // 电源管理寄存器
//#define MPU6050_SMPLRT_DIV     0x19  // 采样率寄存器
//#define MPU6050_CONFIG         0x1A  // 配置寄存器
//#define MPU6050_GYRO_CONFIG    0x1B  // 陀螺仪配置寄存器
//#define MPU6050_ACCEL_CONFIG   0x1C  // 加速度计配置寄存器
//
//

// 写入寄存器的函数
HAL_StatusTypeDef MPU_Write_Register(uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write(&hi2c1, (MPU6050_ADDRESS << 1), reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

// 读取寄存器的函数
HAL_StatusTypeDef MPU_Read_Register(uint8_t reg, uint8_t *data) {
    return HAL_I2C_Mem_Read(&hi2c1, (MPU6050_ADDRESS << 1), reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

// MPU6050 初始化函数
HAL_StatusTypeDef mpu_init(void) {
    HAL_StatusTypeDef ret;

    // 1. 唤醒 MPU6050，清除 SLEEP 位（将 PWR_MGMT_1 寄存器的 SLEEP 位清零）
    ret = MPU_Write_Register(MPU6050_PWR_MGMT_1, 0x00);
    if (ret != HAL_OK) {
        return ret; // 如果初始化失败，返回错误状态
    }

    // 2. 设置采样率，分频值设置为 7（采样率为 1kHz / (1 + 7) = 125Hz）
    ret = MPU_Write_Register(MPU6050_SMPLRT_DIV, 0x07);
    if (ret != HAL_OK) {
        return ret;
    }

    // 3. 配置低通滤波器，DLPF 设置为 42Hz 带宽
    ret = MPU_Write_Register(MPU6050_CONFIG, 0x03);
    if (ret != HAL_OK) {
        return ret;
    }

    // 4. 设置陀螺仪范围为 ±500°/s
    ret = MPU_Write_Register(MPU6050_GYRO_CONFIG, 0x08);
    if (ret != HAL_OK) {
        return ret;
    }

    // 5. 设置加速度计范围为 ±4g
    ret = MPU_Write_Register(MPU6050_ACCEL_CONFIG, 0x08);
    if (ret != HAL_OK) {
        return ret;
    }

    // 初始化成功
    return HAL_OK;
}



// 读取 MPU6050 某个轴向的加速度数据
int16_t MPU_Read_Accel_X(void) {
    uint8_t accel_data[2];  // 存储高、低字节
    int16_t accel_x;

    // 读取 X 轴加速度高字节和低字节
    HAL_I2C_Mem_Read(&hi2c1, (MPU6050_ADDRESS << 1), ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, accel_data, 2, HAL_MAX_DELAY);

    // 组合高低字节为 16 位有符号整数
    accel_x = (int16_t)(accel_data[0] << 8 | accel_data[1]);

    return accel_x;
}

// 读取 Y 轴加速度数据
int16_t MPU_Read_Accel_Y(void) {
    uint8_t accel_data[2];  // 存储高、低字节
    int16_t accel_y;

    // 读取 Y 轴加速度高字节和低字节
    HAL_I2C_Mem_Read(&hi2c1, (MPU6050_ADDRESS << 1), ACCEL_YOUT_H, I2C_MEMADD_SIZE_8BIT, accel_data, 2, HAL_MAX_DELAY);

    // 组合高低字节为 16 位有符号整数
    accel_y = (int16_t)(accel_data[0] << 8 | accel_data[1]);

    return accel_y;
}

// 读取 Z 轴加速度数据
int16_t MPU_Read_Accel_Z(void) {
    uint8_t accel_data[2];  // 存储高、低字节
    int16_t accel_z;

    // 读取 Z 轴加速度高字节和低字节
    HAL_I2C_Mem_Read(&hi2c1, (MPU6050_ADDRESS << 1), ACCEL_ZOUT_H, I2C_MEMADD_SIZE_8BIT, accel_data, 2, HAL_MAX_DELAY);

    // 组合高低字节为 16 位有符号整数
    accel_z = (int16_t)(accel_data[0] << 8 | accel_data[1]);

    return accel_z;
}



