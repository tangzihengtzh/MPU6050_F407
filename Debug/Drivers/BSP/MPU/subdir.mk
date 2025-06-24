################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/MPU/MPU.c 

OBJS += \
./Drivers/BSP/MPU/MPU.o 

C_DEPS += \
./Drivers/BSP/MPU/MPU.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/MPU/%.o Drivers/BSP/MPU/%.su Drivers/BSP/MPU/%.cyclo: ../Drivers/BSP/MPU/%.c Drivers/BSP/MPU/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/KEY_LED -I../Drivers/BSP/LCD -I../Drivers/BSP/MPU -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-MPU

clean-Drivers-2f-BSP-2f-MPU:
	-$(RM) ./Drivers/BSP/MPU/MPU.cyclo ./Drivers/BSP/MPU/MPU.d ./Drivers/BSP/MPU/MPU.o ./Drivers/BSP/MPU/MPU.su

.PHONY: clean-Drivers-2f-BSP-2f-MPU

