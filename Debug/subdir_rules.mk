################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2050/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ASUS/workspace_ccstheia/2024H" -I"C:/Users/ASUS/workspace_ccstheia/2024H/Debug" -I"D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source" -I"D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"C:/Users/ASUS/workspace_ccstheia/2024H/modules" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1285344905: ../empty.syscfg
	@echo 'SysConfig - building file: "$<"'
	"D:/ti/ccs2050/sysconfig_1.26.2/sysconfig_cli.bat" -s "D:/ti/ccs2050/mspm0_sdk_2_10_00_04/.metadata/product.json" --script "C:/Users/ASUS/workspace_ccstheia/2024H/empty.syscfg" -o "." --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-1285344905 ../empty.syscfg
device.opt: build-1285344905
device.cmd.genlibs: build-1285344905
ti_msp_dl_config.c: build-1285344905
ti_msp_dl_config.h: build-1285344905
Event.dot: build-1285344905

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2050/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ASUS/workspace_ccstheia/2024H" -I"C:/Users/ASUS/workspace_ccstheia/2024H/Debug" -I"D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source" -I"D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"C:/Users/ASUS/workspace_ccstheia/2024H/modules" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2050/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ASUS/workspace_ccstheia/2024H" -I"C:/Users/ASUS/workspace_ccstheia/2024H/Debug" -I"D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source" -I"D:/ti/ccs2050/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"C:/Users/ASUS/workspace_ccstheia/2024H/modules" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


