################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/source/FLASH.c \
../src/source/clock_and_mode.c \
../src/source/hal_gpio.c \
../src/source/hal_usart.c \
../src/source/srec_parser.c \
../src/source/srec_queue.c \
../src/source/uart_buffer.c 

OBJS += \
./src/source/FLASH.o \
./src/source/clock_and_mode.o \
./src/source/hal_gpio.o \
./src/source/hal_usart.o \
./src/source/srec_parser.o \
./src/source/srec_queue.o \
./src/source/uart_buffer.o 

C_DEPS += \
./src/source/FLASH.d \
./src/source/clock_and_mode.d \
./src/source/hal_gpio.d \
./src/source/hal_usart.d \
./src/source/srec_parser.d \
./src/source/srec_queue.d \
./src/source/uart_buffer.d 


# Each subdirectory must supply rules for building sources it contributes
src/source/%.o: ../src/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/source/FLASH.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


