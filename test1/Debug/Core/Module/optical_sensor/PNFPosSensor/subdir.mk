################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Module/optical_sensor/PNFPosSensor/PNFPosSensor.cpp 

OBJS += \
./Core/Module/optical_sensor/PNFPosSensor/PNFPosSensor.o 

CPP_DEPS += \
./Core/Module/optical_sensor/PNFPosSensor/PNFPosSensor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Module/optical_sensor/PNFPosSensor/%.o Core/Module/optical_sensor/PNFPosSensor/%.su: ../Core/Module/optical_sensor/PNFPosSensor/%.cpp Core/Module/optical_sensor/PNFPosSensor/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Middlewares/Third_Party/LwIP/system/arch -I../Middlewares/Third_Party/LwIP/src/include -I../LWIP/App -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Core/Inc -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/system -I../Drivers/BSP/Components/lan8742 -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Drivers/CMSIS/Include -I../LWIP/Target -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/netif -I"/home/studio3s/st/STM32CubeIDE/workspace_1.9.0/test1/Core/Module" -I"/home/studio3s/st/STM32CubeIDE/workspace_1.9.0/test1/Core/Module/ethernet" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Module-2f-optical_sensor-2f-PNFPosSensor

clean-Core-2f-Module-2f-optical_sensor-2f-PNFPosSensor:
	-$(RM) ./Core/Module/optical_sensor/PNFPosSensor/PNFPosSensor.d ./Core/Module/optical_sensor/PNFPosSensor/PNFPosSensor.o ./Core/Module/optical_sensor/PNFPosSensor/PNFPosSensor.su

.PHONY: clean-Core-2f-Module-2f-optical_sensor-2f-PNFPosSensor

