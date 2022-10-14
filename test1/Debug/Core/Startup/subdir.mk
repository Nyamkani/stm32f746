################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f746zetx.s 

S_DEPS += \
./Core/Startup/startup_stm32f746zetx.d 

OBJS += \
./Core/Startup/startup_stm32f746zetx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g -DDEBUG -c -I"/home/studio3s/st/STM32CubeIDE/workspace_1.9.0/test1/Core/Module" -I"/home/studio3s/st/STM32CubeIDE/workspace_1.9.0/test1/Middlewares/Third_Party/FreeRTOS" -I"/home/studio3s/st/STM32CubeIDE/workspace_1.9.0/test1/LWIP" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f746zetx.d ./Core/Startup/startup_stm32f746zetx.o

.PHONY: clean-Core-2f-Startup

