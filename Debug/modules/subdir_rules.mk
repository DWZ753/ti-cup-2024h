################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
modules/%.o: ../modules/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2050/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ASUS/workspace_ccstheia/2024H" -I"C:/Users/ASUS/workspace_ccstheia/2024H/Debug" -I"D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source" -I"D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"C:/Users/ASUS/workspace_ccstheia/2024H/modules" -gdwarf-3 -Wall -MMD -MP -MF"modules/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


