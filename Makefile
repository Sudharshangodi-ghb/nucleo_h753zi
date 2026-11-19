
# Project
TARGET      = nucleo_h753zi
BUILD_DIR   = build

# Toolchain
CC          = arm-none-eabi-gcc
OBJCOPY     = arm-none-eabi-objcopy
SIZE        = arm-none-eabi-size

# Target architecture: STM32H753ZI (Cortex-M7)
CPU_FLAGS   = -mcpu=cortex-m7 -mthumb \
              -mfpu=fpv5-d16 -mfloat-abi=hard

# Preprocessor definitions
DEFINES     = -DSTM32H753xx \
              -DUSE_PWR_LDO_SUPPLY

# Include directories
INCLUDES    = -ICore/Inc \
              -ICMSIS/Include \
              -ICMSIS/Device/ST/STM32H7xx/Include \
              -IDrivers/Ethernet \
              -IDrivers/GPIO \
              -IDrivers/UART

# C compiler flags
CFLAGS      = $(CPU_FLAGS) \
              $(DEFINES) \
              $(INCLUDES) \
              -std=c11 \
              -O0 \
              -g3 \
              -Wall \
              -Wextra \
              -MMD \
              -MP \
              -ffunction-sections \
              -fdata-sections

# Linker script
LDSCRIPT    = Linker/STM32H753ZITX_FLASH.ld

# Linker flags
LDFLAGS     = $(CPU_FLAGS) \
              -T$(LDSCRIPT) \
              -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
              -Wl,--gc-sections \
              -Wl,--print-memory-usage \
              -Wl,--fatal-warnings \
              -nostartfiles

# Application source files
C_SOURCES   = Core/Src/main.c \
              Core/Src/app.c \
              Core/Src/system_stm32h7xx.c \
              Drivers/Ethernet/ethernet_mac.c \
              Drivers/GPIO/gpio.c \
              Drivers/GPIO/led.c \
              Drivers/UART/uart3.c

ASM_SOURCES = Startup/startup_stm32h753xx.s

# Object files
OBJECTS     = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
OBJECTS    += $(patsubst %.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
DEPFILES    = $(patsubst %.c,$(BUILD_DIR)/%.d,$(C_SOURCES))

# Default target
all: $(BUILD_DIR)/$(TARGET).elf \
     $(BUILD_DIR)/$(TARGET).bin

# Compile C source files
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble startup file
$(BUILD_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) $(CPU_FLAGS) -c $< -o $@

# Link ELF
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(SIZE) $@

# Generate binary
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Clean build output
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPFILES)

.PHONY: all clean
