################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sourse/FLASH.c \
../src/sourse/Led_control.c \
../src/sourse/clock_and_mode.c \
../src/sourse/hal_gpio.c \
../src/sourse/hal_usart.c \
../src/sourse/srec_parser.c \
../src/sourse/srec_queue.c \
../src/sourse/uart_queue.c 

OBJS += \
./src/sourse/FLASH.o \
./src/sourse/Led_control.o \
./src/sourse/clock_and_mode.o \
./src/sourse/hal_gpio.o \
./src/sourse/hal_usart.o \
./src/sourse/srec_parser.o \
./src/sourse/srec_queue.o \
./src/sourse/uart_queue.o 

C_DEPS += \
./src/sourse/FLASH.d \
./src/sourse/Led_control.d \
./src/sourse/clock_and_mode.d \
./src/sourse/hal_gpio.d \
./src/sourse/hal_usart.d \
./src/sourse/srec_parser.d \
./src/sourse/srec_queue.d \
./src/sourse/uart_queue.d 


# Each subdirectory must supply rules for building sources it contributes
src/sourse/%.o: ../src/sourse/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/sourse/FLASH.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


