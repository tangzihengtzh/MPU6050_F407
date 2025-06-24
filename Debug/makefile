################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/FatFs/src/option/subdir.mk
-include Middlewares/Third_Party/FatFs/src/subdir.mk
-include FATFS/Target/subdir.mk
-include FATFS/App/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Drivers/BSP/MPU/subdir.mk
-include Drivers/BSP/LCD/subdir.mk
-include Drivers/BSP/KEY_LED/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := MPU6050_acceleration_capture
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
MPU6050_acceleration_capture.elf \

MAP_FILES += \
MPU6050_acceleration_capture.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
MPU6050_acceleration_capture.list \

OBJCOPY_HEX += \
MPU6050_acceleration_capture.hex \


# All Target
all: main-build

# Main-build Target
main-build: MPU6050_acceleration_capture.elf secondary-outputs

# Tool invocations
MPU6050_acceleration_capture.elf MPU6050_acceleration_capture.map: $(OBJS) $(USER_OBJS) E:\STM32_PRJ\KW407\Take_you_read_STM32Cube_efficient_development_tutorial-master\Demo8_1TFTLCD_V3\MPU6050_acceleration_capture\STM32F407ZGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "MPU6050_acceleration_capture.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\STM32_PRJ\KW407\Take_you_read_STM32Cube_efficient_development_tutorial-master\Demo8_1TFTLCD_V3\MPU6050_acceleration_capture\STM32F407ZGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="MPU6050_acceleration_capture.map" -Wl,--gc-sections -static -u _printf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

MPU6050_acceleration_capture.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "MPU6050_acceleration_capture.list"
	@echo 'Finished building: $@'
	@echo ' '

MPU6050_acceleration_capture.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "MPU6050_acceleration_capture.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) MPU6050_acceleration_capture.elf MPU6050_acceleration_capture.hex MPU6050_acceleration_capture.list MPU6050_acceleration_capture.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
