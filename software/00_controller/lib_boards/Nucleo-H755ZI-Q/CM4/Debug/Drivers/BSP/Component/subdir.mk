################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/jarek/Projects/vcs/UniController/software/00_controller/lib_boards/Nucleo-H755ZI-Q/Drivers/BSP/Components/lan8742/lan8742.c 

OBJS += \
./Drivers/BSP/Component/lan8742.o 

C_DEPS += \
./Drivers/BSP/Component/lan8742.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Component/lan8742.o: /home/jarek/Projects/vcs/UniController/software/00_controller/lib_boards/Nucleo-H755ZI-Q/Drivers/BSP/Components/lan8742/lan8742.c Drivers/BSP/Component/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_NUCLEO_64 -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/BSP/STM32H7xx_Nucleo -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Component

clean-Drivers-2f-BSP-2f-Component:
	-$(RM) ./Drivers/BSP/Component/lan8742.cyclo ./Drivers/BSP/Component/lan8742.d ./Drivers/BSP/Component/lan8742.o ./Drivers/BSP/Component/lan8742.su

.PHONY: clean-Drivers-2f-BSP-2f-Component

