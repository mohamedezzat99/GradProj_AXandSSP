################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
D:\Downloads\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.4\libraries\SPI\src\SPI.cpp 

LINK_OBJ += \
.\libraries\SPI\src\SPI.cpp.o 

CPP_DEPS += \
.\libraries\SPI\src\SPI.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries\SPI\src\SPI.cpp.o: D:\Downloads\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.4\libraries\SPI\src\SPI.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"D:\Downloads\Sloeber\arduinoPlugin\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7/bin/avr-g++" -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR     -I"D:\Downloads\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.4\cores\arduino" -I"D:\Downloads\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.4\variants\mega" -I"C:\Users\Mohamed Ezzat\Documents\Arduino\libraries\RF24" -I"D:\Downloads\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.4\libraries\SPI\src" -I"C:\Users\Mohamed Ezzat\Documents\Arduino\libraries\RF24\utility" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"
	@echo 'Finished building: $<'
	@echo ' '


