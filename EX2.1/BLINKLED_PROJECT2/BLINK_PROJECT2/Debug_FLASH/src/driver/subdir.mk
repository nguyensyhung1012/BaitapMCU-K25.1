################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/driver/Driver_NVIC.c \
../src/driver/clock_and_mode.c 

OBJS += \
./src/driver/Driver_NVIC.o \
./src/driver/clock_and_mode.o 

C_DEPS += \
./src/driver/Driver_NVIC.d \
./src/driver/clock_and_mode.d 


# Each subdirectory must supply rules for building sources it contributes
src/driver/%.o: ../src/driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/driver/Driver_NVIC.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


