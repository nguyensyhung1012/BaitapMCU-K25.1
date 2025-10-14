################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/driver/Driver_GPIO.c \
../src/driver/Driver_NVIC.c \
../src/driver/Driver_PORT_S32K144.c \
../src/driver/Driver_USART.c 

OBJS += \
./src/driver/Driver_GPIO.o \
./src/driver/Driver_NVIC.o \
./src/driver/Driver_PORT_S32K144.o \
./src/driver/Driver_USART.o 

C_DEPS += \
./src/driver/Driver_GPIO.d \
./src/driver/Driver_NVIC.d \
./src/driver/Driver_PORT_S32K144.d \
./src/driver/Driver_USART.d 


# Each subdirectory must supply rules for building sources it contributes
src/driver/%.o: ../src/driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/driver/Driver_GPIO.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


