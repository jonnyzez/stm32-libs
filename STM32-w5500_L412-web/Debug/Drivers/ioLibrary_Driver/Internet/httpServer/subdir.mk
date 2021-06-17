################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.c \
../Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.c \
../Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.c 

OBJS += \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.o \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.o \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.d \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.d \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.o: ../Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.c Drivers/ioLibrary_Driver/Internet/httpServer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L412xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.o: ../Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.c Drivers/ioLibrary_Driver/Internet/httpServer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L412xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.o: ../Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.c Drivers/ioLibrary_Driver/Internet/httpServer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L412xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

