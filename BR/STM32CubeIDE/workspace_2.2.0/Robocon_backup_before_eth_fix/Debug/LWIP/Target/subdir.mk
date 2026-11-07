################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LWIP/Target/ethernetif.c 

OBJS += \
./LWIP/Target/ethernetif.o 

C_DEPS += \
./LWIP/Target/ethernetif.d 


# Each subdirectory must supply rules for building sources it contributes
LWIP/Target/%.o LWIP/Target/%.su LWIP/Target/%.cyclo: ../LWIP/Target/%.c LWIP/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F7 -DSTM32F767ZITx -DSTM32F767xx -c -I../Inc -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Include" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Inc" -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/STM32F7xx_HAL_Driver/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Device/ST/STM32F7xx/Include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/LWIP/App -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/LWIP/Target -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/include/lwip -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/system -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/core -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/core/ipv4 -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/netif -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-LWIP-2f-Target

clean-LWIP-2f-Target:
	-$(RM) ./LWIP/Target/ethernetif.cyclo ./LWIP/Target/ethernetif.d ./LWIP/Target/ethernetif.o ./LWIP/Target/ethernetif.su

.PHONY: clean-LWIP-2f-Target

