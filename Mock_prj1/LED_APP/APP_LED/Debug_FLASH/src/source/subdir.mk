################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/source/Driver_NVIC.c 

OBJS += \
./src/source/Driver_NVIC.o 

C_DEPS += \
./src/source/Driver_NVIC.d 


# Each subdirectory must supply rules for building sources it contributes
src/source/%.o: ../src/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/source/Driver_NVIC.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


