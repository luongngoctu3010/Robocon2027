################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Src/startup_stm32f767xx.s 

C_SRCS += \
../Src/gpio.c \
../Src/hoverboard.c \
../Src/main.c \
../Src/stm32f7xx_hal_msp.c \
../Src/stm32f7xx_it.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_stm32f7xx.c 

OBJS += \
./Src/gpio.o \
./Src/hoverboard.o \
./Src/main.o \
./Src/startup_stm32f767xx.o \
./Src/stm32f7xx_hal_msp.o \
./Src/stm32f7xx_it.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_stm32f7xx.o 

S_DEPS += \
./Src/startup_stm32f767xx.d 

C_DEPS += \
./Src/gpio.d \
./Src/hoverboard.d \
./Src/main.d \
./Src/stm32f7xx_hal_msp.d \
./Src/stm32f7xx_it.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_stm32f7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F7 -DSTM32F767ZITx -DSTM32F767xx -c -I../Inc -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Include" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Inc" -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/STM32F7xx_HAL_Driver/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Device/ST/STM32F7xx/Include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/LWIP/App -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/LWIP/Target -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/include/lwip -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/system -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/core -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/core/ipv4 -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/netif -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/%.o: ../Src/%.s Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -DDEBUG -DSTM32F767xx -c -I/Robocon/Drivers/STM32F7xx_HAL_Driver/Inc -I/Robocon/Drivers/CMSIS/Device/ST/STM32F7xx/Include -I/Robocon/Drivers/CMSIS/Include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/STM32F7xx_HAL_Driver/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Device/ST/STM32F7xx/Include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Include -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon_USB_Test/Middlewares/Third_Party/LwIP/src/include" -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/LWIP/App -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/LWIP/Target -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/include/lwip -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/system -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/core -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/core/ipv4 -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/netif -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/gpio.cyclo ./Src/gpio.d ./Src/gpio.o ./Src/gpio.su ./Src/hoverboard.cyclo ./Src/hoverboard.d ./Src/hoverboard.o ./Src/hoverboard.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/startup_stm32f767xx.d ./Src/startup_stm32f767xx.o ./Src/stm32f7xx_hal_msp.cyclo ./Src/stm32f7xx_hal_msp.d ./Src/stm32f7xx_hal_msp.o ./Src/stm32f7xx_hal_msp.su ./Src/stm32f7xx_it.cyclo ./Src/stm32f7xx_it.d ./Src/stm32f7xx_it.o ./Src/stm32f7xx_it.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/system_stm32f7xx.cyclo ./Src/system_stm32f7xx.d ./Src/system_stm32f7xx.o ./Src/system_stm32f7xx.su

.PHONY: clean-Src

