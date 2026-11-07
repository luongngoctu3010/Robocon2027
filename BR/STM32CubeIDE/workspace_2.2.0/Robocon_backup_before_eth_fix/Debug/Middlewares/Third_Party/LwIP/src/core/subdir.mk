################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/LwIP/src/core/altcp.c \
../Middlewares/Third_Party/LwIP/src/core/altcp_alloc.c \
../Middlewares/Third_Party/LwIP/src/core/altcp_tcp.c \
../Middlewares/Third_Party/LwIP/src/core/def.c \
../Middlewares/Third_Party/LwIP/src/core/dns.c \
../Middlewares/Third_Party/LwIP/src/core/inet_chksum.c \
../Middlewares/Third_Party/LwIP/src/core/init.c \
../Middlewares/Third_Party/LwIP/src/core/ip.c \
../Middlewares/Third_Party/LwIP/src/core/mem.c \
../Middlewares/Third_Party/LwIP/src/core/memp.c \
../Middlewares/Third_Party/LwIP/src/core/netif.c \
../Middlewares/Third_Party/LwIP/src/core/pbuf.c \
../Middlewares/Third_Party/LwIP/src/core/raw.c \
../Middlewares/Third_Party/LwIP/src/core/stats.c \
../Middlewares/Third_Party/LwIP/src/core/sys.c \
../Middlewares/Third_Party/LwIP/src/core/tcp.c \
../Middlewares/Third_Party/LwIP/src/core/tcp_in.c \
../Middlewares/Third_Party/LwIP/src/core/tcp_out.c \
../Middlewares/Third_Party/LwIP/src/core/timeouts.c \
../Middlewares/Third_Party/LwIP/src/core/udp.c 

OBJS += \
./Middlewares/Third_Party/LwIP/src/core/altcp.o \
./Middlewares/Third_Party/LwIP/src/core/altcp_alloc.o \
./Middlewares/Third_Party/LwIP/src/core/altcp_tcp.o \
./Middlewares/Third_Party/LwIP/src/core/def.o \
./Middlewares/Third_Party/LwIP/src/core/dns.o \
./Middlewares/Third_Party/LwIP/src/core/inet_chksum.o \
./Middlewares/Third_Party/LwIP/src/core/init.o \
./Middlewares/Third_Party/LwIP/src/core/ip.o \
./Middlewares/Third_Party/LwIP/src/core/mem.o \
./Middlewares/Third_Party/LwIP/src/core/memp.o \
./Middlewares/Third_Party/LwIP/src/core/netif.o \
./Middlewares/Third_Party/LwIP/src/core/pbuf.o \
./Middlewares/Third_Party/LwIP/src/core/raw.o \
./Middlewares/Third_Party/LwIP/src/core/stats.o \
./Middlewares/Third_Party/LwIP/src/core/sys.o \
./Middlewares/Third_Party/LwIP/src/core/tcp.o \
./Middlewares/Third_Party/LwIP/src/core/tcp_in.o \
./Middlewares/Third_Party/LwIP/src/core/tcp_out.o \
./Middlewares/Third_Party/LwIP/src/core/timeouts.o \
./Middlewares/Third_Party/LwIP/src/core/udp.o 

C_DEPS += \
./Middlewares/Third_Party/LwIP/src/core/altcp.d \
./Middlewares/Third_Party/LwIP/src/core/altcp_alloc.d \
./Middlewares/Third_Party/LwIP/src/core/altcp_tcp.d \
./Middlewares/Third_Party/LwIP/src/core/def.d \
./Middlewares/Third_Party/LwIP/src/core/dns.d \
./Middlewares/Third_Party/LwIP/src/core/inet_chksum.d \
./Middlewares/Third_Party/LwIP/src/core/init.d \
./Middlewares/Third_Party/LwIP/src/core/ip.d \
./Middlewares/Third_Party/LwIP/src/core/mem.d \
./Middlewares/Third_Party/LwIP/src/core/memp.d \
./Middlewares/Third_Party/LwIP/src/core/netif.d \
./Middlewares/Third_Party/LwIP/src/core/pbuf.d \
./Middlewares/Third_Party/LwIP/src/core/raw.d \
./Middlewares/Third_Party/LwIP/src/core/stats.d \
./Middlewares/Third_Party/LwIP/src/core/sys.d \
./Middlewares/Third_Party/LwIP/src/core/tcp.d \
./Middlewares/Third_Party/LwIP/src/core/tcp_in.d \
./Middlewares/Third_Party/LwIP/src/core/tcp_out.d \
./Middlewares/Third_Party/LwIP/src/core/timeouts.d \
./Middlewares/Third_Party/LwIP/src/core/udp.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/LwIP/src/core/%.o Middlewares/Third_Party/LwIP/src/core/%.su Middlewares/Third_Party/LwIP/src/core/%.cyclo: ../Middlewares/Third_Party/LwIP/src/core/%.c Middlewares/Third_Party/LwIP/src/core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F7 -DSTM32F767ZITx -DSTM32F767xx -c -I../Inc -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Include" -I"/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Inc" -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/STM32F7xx_HAL_Driver/Inc -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Device/ST/STM32F7xx/Include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Drivers/CMSIS/Include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/include -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/LWIP/App -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/LWIP/Target -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/include/lwip -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/system -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/core -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/core/ipv4 -I/home/luongngoctu/STM32CubeIDE/workspace_2.2.0/Robocon/Middlewares/Third_Party/LwIP/src/netif -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-LwIP-2f-src-2f-core

clean-Middlewares-2f-Third_Party-2f-LwIP-2f-src-2f-core:
	-$(RM) ./Middlewares/Third_Party/LwIP/src/core/altcp.cyclo ./Middlewares/Third_Party/LwIP/src/core/altcp.d ./Middlewares/Third_Party/LwIP/src/core/altcp.o ./Middlewares/Third_Party/LwIP/src/core/altcp.su ./Middlewares/Third_Party/LwIP/src/core/altcp_alloc.cyclo ./Middlewares/Third_Party/LwIP/src/core/altcp_alloc.d ./Middlewares/Third_Party/LwIP/src/core/altcp_alloc.o ./Middlewares/Third_Party/LwIP/src/core/altcp_alloc.su ./Middlewares/Third_Party/LwIP/src/core/altcp_tcp.cyclo ./Middlewares/Third_Party/LwIP/src/core/altcp_tcp.d ./Middlewares/Third_Party/LwIP/src/core/altcp_tcp.o ./Middlewares/Third_Party/LwIP/src/core/altcp_tcp.su ./Middlewares/Third_Party/LwIP/src/core/def.cyclo ./Middlewares/Third_Party/LwIP/src/core/def.d ./Middlewares/Third_Party/LwIP/src/core/def.o ./Middlewares/Third_Party/LwIP/src/core/def.su ./Middlewares/Third_Party/LwIP/src/core/dns.cyclo ./Middlewares/Third_Party/LwIP/src/core/dns.d ./Middlewares/Third_Party/LwIP/src/core/dns.o ./Middlewares/Third_Party/LwIP/src/core/dns.su ./Middlewares/Third_Party/LwIP/src/core/inet_chksum.cyclo ./Middlewares/Third_Party/LwIP/src/core/inet_chksum.d ./Middlewares/Third_Party/LwIP/src/core/inet_chksum.o ./Middlewares/Third_Party/LwIP/src/core/inet_chksum.su ./Middlewares/Third_Party/LwIP/src/core/init.cyclo ./Middlewares/Third_Party/LwIP/src/core/init.d ./Middlewares/Third_Party/LwIP/src/core/init.o ./Middlewares/Third_Party/LwIP/src/core/init.su ./Middlewares/Third_Party/LwIP/src/core/ip.cyclo ./Middlewares/Third_Party/LwIP/src/core/ip.d ./Middlewares/Third_Party/LwIP/src/core/ip.o ./Middlewares/Third_Party/LwIP/src/core/ip.su ./Middlewares/Third_Party/LwIP/src/core/mem.cyclo ./Middlewares/Third_Party/LwIP/src/core/mem.d ./Middlewares/Third_Party/LwIP/src/core/mem.o ./Middlewares/Third_Party/LwIP/src/core/mem.su ./Middlewares/Third_Party/LwIP/src/core/memp.cyclo ./Middlewares/Third_Party/LwIP/src/core/memp.d ./Middlewares/Third_Party/LwIP/src/core/memp.o ./Middlewares/Third_Party/LwIP/src/core/memp.su ./Middlewares/Third_Party/LwIP/src/core/netif.cyclo ./Middlewares/Third_Party/LwIP/src/core/netif.d ./Middlewares/Third_Party/LwIP/src/core/netif.o ./Middlewares/Third_Party/LwIP/src/core/netif.su ./Middlewares/Third_Party/LwIP/src/core/pbuf.cyclo ./Middlewares/Third_Party/LwIP/src/core/pbuf.d ./Middlewares/Third_Party/LwIP/src/core/pbuf.o ./Middlewares/Third_Party/LwIP/src/core/pbuf.su ./Middlewares/Third_Party/LwIP/src/core/raw.cyclo ./Middlewares/Third_Party/LwIP/src/core/raw.d ./Middlewares/Third_Party/LwIP/src/core/raw.o ./Middlewares/Third_Party/LwIP/src/core/raw.su ./Middlewares/Third_Party/LwIP/src/core/stats.cyclo ./Middlewares/Third_Party/LwIP/src/core/stats.d ./Middlewares/Third_Party/LwIP/src/core/stats.o ./Middlewares/Third_Party/LwIP/src/core/stats.su ./Middlewares/Third_Party/LwIP/src/core/sys.cyclo ./Middlewares/Third_Party/LwIP/src/core/sys.d ./Middlewares/Third_Party/LwIP/src/core/sys.o ./Middlewares/Third_Party/LwIP/src/core/sys.su ./Middlewares/Third_Party/LwIP/src/core/tcp.cyclo ./Middlewares/Third_Party/LwIP/src/core/tcp.d ./Middlewares/Third_Party/LwIP/src/core/tcp.o ./Middlewares/Third_Party/LwIP/src/core/tcp.su ./Middlewares/Third_Party/LwIP/src/core/tcp_in.cyclo ./Middlewares/Third_Party/LwIP/src/core/tcp_in.d ./Middlewares/Third_Party/LwIP/src/core/tcp_in.o ./Middlewares/Third_Party/LwIP/src/core/tcp_in.su ./Middlewares/Third_Party/LwIP/src/core/tcp_out.cyclo ./Middlewares/Third_Party/LwIP/src/core/tcp_out.d ./Middlewares/Third_Party/LwIP/src/core/tcp_out.o ./Middlewares/Third_Party/LwIP/src/core/tcp_out.su ./Middlewares/Third_Party/LwIP/src/core/timeouts.cyclo ./Middlewares/Third_Party/LwIP/src/core/timeouts.d ./Middlewares/Third_Party/LwIP/src/core/timeouts.o ./Middlewares/Third_Party/LwIP/src/core/timeouts.su ./Middlewares/Third_Party/LwIP/src/core/udp.cyclo ./Middlewares/Third_Party/LwIP/src/core/udp.d ./Middlewares/Third_Party/LwIP/src/core/udp.o ./Middlewares/Third_Party/LwIP/src/core/udp.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-LwIP-2f-src-2f-core

