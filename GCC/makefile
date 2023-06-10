default: all
include makefile_color
include makefile_version

###############################################################################
# Main application file name
###############################################################################
TARGET := $(notdir $(shell dirname $(shell pwd)))
PHONY :=
MAKEFLAGS += --silent


###############################################################################
# Build path
###############################################################################
ifeq ($(RELEASE), 1)
BUILD_DIR := ../build/release
else
BUILD_DIR := ../build/debug
DEBUG := 1
endif

SRC_DIR := ../code
LIB_FOLDER := Drivers


###############################################################################
# Source
###############################################################################
# Defines
DEFINES_C := STM32F303xC
DEFINES_C += USE_FULL_LL_DRIVER
DEFINES_C += USE_HAL_DRIVER
DEFINES_C += HSE_STARTUP_TIMEOUT=100
DEFINES_C += LSE_STARTUP_TIMEOUT=5000
DEFINES_C += LSE_VALUE=32768
DEFINES_C += EXTERNAL_CLOCK_VALUE=8000000
DEFINES_C += EXTERNAL_CLOCK_VALUE=8000000
DEFINES_C += PREFETCH_ENABLE=1
DEFINES_C += LSI_VALUE=40000
DEFINES_C += VERSION_STR=\"$(VERSION_STR)\"
DEFINES_C += VERSION=$(VERSION_NUM)

# includes
INCLUDES := $(shell find $(SRC_DIR) -type d)
INCLUDES_LIB := $(shell find $(SRC_DIR)/$(LIB_FOLDER) -type d)
INCLUDES_SRC := $(filter-out $(INCLUDES_LIB),$(INCLUDES))
INCLUDES := $(INCLUDES_SRC) $(INCLUDES_LIB)

INCLUDES_CC :=

# ASM sources
SOURCES_A := startup_stm32f303xc.s

# C sources
SOURCES_C := $(wildcard $(addsuffix /*.c, $(INCLUDES_SRC)))

# C lib sources
SOURCES_C_LIB := $(wildcard $(addsuffix /*.c, $(INCLUDES_LIB)))


###############################################################################
# Compilers and Utilities binaries
###############################################################################
PREFIX = arm-none-eabi-
CC_PATH ?= C:/toolchain/arm-gnu-toolchain-12.2.mpacbti-rel1-mingw-w64-i686-arm-none-eabi/bin

AS 		:= $(CC_PATH)/$(PREFIX)gcc -xassembler-with-cpp
CC 		:= $(CC_PATH)/$(PREFIX)gcc.exe
LD 		:= $(CC_PATH)/$(PREFIX)gcc.exe
OBJCOPY := $(CC_PATH)/$(PREFIX)objcopy
OBJDUMP := $(CC_PATH)/$(PREFIX)objdump
SZ 		:= $(CC_PATH)/$(PREFIX)size
CC_VERSION := $(shell $(CC) --version)


###############################################################################
# CFLAGS
###############################################################################
# CPU
CPU := -mcpu=cortex-m4

# FPU
FPU := -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# MCU
MCU := $(CPU) $(FPU) -mthumb

# Optimization and Debug level
ifeq ($(DEBUG), 1)
DEFINES_C += DEBUG
OPT := -O1 --debug
else
DEFINES_C += DEBUG
OPT := -Os
endif

# Compile flags
FLAGS := $(MCU)

AFLAGS := $(FLAGS)

FLAGS_C := $(FLAGS)
FLAGS_C += -ffunction-sections
FLAGS_C += -fdata-sections
FLAGS_C += -fshort-enums
FLAGS_C += -funsigned-char
FLAGS_C += -fpeel-loops
FLAGS_C += -std=gnu17
FLAGS_C += $(OPT)
FLAGS_C += -MMD -MP

FLAGS_C_W := -Wall
FLAGS_C_W += -Winline
FLAGS_C_W += -Wextra
FLAGS_C_W += -pedantic

FLAGS_DEF := $(addprefix -D, $(DEFINES_C))
FLAGS_INC := $(addprefix -I, $(INCLUDES)) $(addprefix -I, $(INCLUDES_CC))


###############################################################################
# FLAGS_LD
###############################################################################
# link script
LSCRIPT := STM32F303CCTx_FLASH.ld

# libraries
LIBS := -lc -lm -lnosys 
LIBDIR := 
FLAGS_LD := $(MCU) -T$(LSCRIPT) $(LIBDIR) $(LIBS)
FLAGS_LD += --specs=nano.specs 
FLAGS_LD += --specs=nosys.specs 
# FLAGS_LD += -u _scanf_float 
# FLAGS_LD += -u _printf_float
FLAGS_LD += -Wl,--gc-sections
FLAGS_LD += -Wl,--no-warn-rwx-segments
FLAGS_LD += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref 


###############################################################################
# HFLAGS
###############################################################################
# Options for HEX file generation
HFLAGS := -O ihex

###############################################################################
# build the application
###############################################################################
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES_C:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES_C_LIB:.c=.lo)))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES_A:.s=.o)))
vpath %.c $(sort $(INCLUDES))
vpath %.s $(sort $(INCLUDES))

# Build all
all: build
	@echo
	$(SZ) $(BUILD_DIR)/$(TARGET).elf
	@echo
	$(call echo_green,"Listing:") $(BUILD_DIR)/$(TARGET).lss

build: check_cc info $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).lss

rebuild: clean build

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJCOPY) $(HFLAGS) $< $@

$(BUILD_DIR)/%.lss: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJDUMP) -S $< > $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile	
	$(CC) $(OBJECTS) $(FLAGS_LD) -o $@

$(BUILD_DIR)/%.lo: %.c Makefile | $(BUILD_DIR)
	@echo $<
	$(CC) -c $(FLAGS_C) $(FLAGS_INC) $(FLAGS_DEF) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo $<
	$(CC) -c $(FLAGS_C) $(FLAGS_C_W) $(FLAGS_INC) $(FLAGS_DEF) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo $<
	$(AS) -c $(AFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	@echo -rm -fR $(dir $(BUILD_DIR))
	@-rm -fR $(dir $(BUILD_DIR))

utility="C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"
flash: $(BUILD_DIR)/$(TARGET).hex
	$(utility) -P $< 0x08000000
	$(utility) -Rst -Run >&-
	@echo $(COLOR_GREEN)"Appication Run"$(COLOR_NC)

erase:
	$(utility) -ME >&-
	@echo $(COLOR_GREEN)"Flash memory erased"$(COLOR_NC)

info: version_info
	$(call echo_green,"Target:") $(TARGET)
	$(call echo_green,"Build to:") $(BUILD_DIR)
	$(call echo_green,Compiler version:) "$(CC_VERSION)"
	$(call echo_green,Compiler flags:) $(FLAGS_C)
	$(call echo_green,Defines:) $(DEFINES_C)
	$(call echo_green,Linking flags:) $(FLAGS_LD)
	@echo

PHONY += test
test :
	@echo ${BUILD_DIR}
	@echo ${OBJECTS}

PHONY += check_cc
check_cc:
ifeq ($(wildcard $(CC_PATH)),)
	$(error Compiler path: $(CC_PATH) not found)
endif


###############################################################################
# dependencies
###############################################################################
-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: $(PHONY)
