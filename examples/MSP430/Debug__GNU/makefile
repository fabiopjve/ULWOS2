################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv8/tools/compiler/msp430-gcc-8.3.1.25_win32

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./blink.o" \
"./ULWOS2/src/ULWOS2.o" \
"./ULWOS2/src/ULWOS2_HAL.o" \
-T"../msp430f2013.ld" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lgcc \
-lc \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include ULWOS2/src/subdir_vars.mk
-include subdir_rules.mk
-include ULWOS2/src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
ULWOS2_Blinker_MSP430.out \

EXE_OUTPUTS__QUOTED += \
"ULWOS2_Blinker_MSP430.out" \

BIN_OUTPUTS += \
ULWOS2_Blinker_MSP430.hex \

BIN_OUTPUTS__QUOTED += \
"ULWOS2_Blinker_MSP430.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(LD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "ULWOS2_Blinker_MSP430.out"

# Tool invocations
ULWOS2_Blinker_MSP430.out: $(OBJS) $(CMD_SRCS) $(LD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"C:/ti/ccsv8/tools/compiler/msp430-gcc-8.3.1.25_win32/bin/msp430-elf-gcc.exe" -mhwmult=none -DULWOS2_TARGET=ULWOS2_TARGET_MSP430 -DULWOS2_MAX_THREADS=4 -Os -ffunction-sections -g -gdwarf-3 -gstrict-dwarf -Wall -mmcu=msp430f2013 -Wl,-Map,"ULWOS2_Blinker_MSP430.map" -Wl,--gc-sections -L"C:/ti/ccsv8/ccs_base/msp430/include_gcc" -o"ULWOS2_Blinker_MSP430.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

ULWOS2_Blinker_MSP430.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: GNU Objcopy Utility'
	"C:/ti/ccsv8/tools/compiler/msp430-gcc-8.3.1.25_win32/bin/msp430-elf-objcopy.exe"  $(EXE_OUTPUTS__QUOTED) "ULWOS2_Blinker_MSP430.hex"
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "blink.o" "ULWOS2\src\ULWOS2.o" "ULWOS2\src\ULWOS2_HAL.o" 
	-$(RM) "blink.d" "ULWOS2\src\ULWOS2.d" "ULWOS2\src\ULWOS2_HAL.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

