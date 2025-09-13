# STM32F10x Makefile for arm-none-eabi-gcc
# arm-none-eabi-gcc (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119

# Toolchain
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
LD      = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE    = arm-none-eabi-size

# MCU and FPU settings (adjust as needed)
MCU     = cortex-m3
DEFS    = -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
LDSCRIPT= LinkerScript.ld

# Directories
SRC_DIRS = src STM32F10x_StdPeriph_Driver/src CMSIS/CM3/CoreSupport CMSIS/CM3/DeviceSupport/ST/STM32F10x/

STARTUP = startup/startup_stm32.s

# Add startup object to objects list
OBJS += $(STARTUP:.s=.o)

# Find all source files
SRCS = $(shell find $(SRC_DIRS) -name '*.c')
OBJS = $(SRCS:.c=.o)

# Includes
INCLUDES = -I./src \
           -I./STM32F10x_StdPeriph_Driver/src \
           -I./CMSIS/CM3/CoreSupport \
           -I./CMSIS/CM3/DeviceSupport/ST/STM32F10x

INCLUDES += -I./src/freertos/include \
			-I./STM32F10x_StdPeriph_Driver/inc \
			-I./src/mcu_peripherals \
			-I./src/modules \
			-I./src/utils \
			-I./src/system_config

# Flags
CFLAGS  = -mcpu=$(MCU) -mthumb -O2 $(DEFS) $(INCLUDES) -Wall -ffunction-sections -fdata-sections -Wpedantic
LDFLAGS = -T$(LDSCRIPT) -mcpu=$(MCU) -mthumb -Wl,--gc-sections -lc -lm

# Output
TARGET = build/growhome.elf

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p build
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(OBJCOPY) -O ihex $@ build/growhome.hex
	$(OBJCOPY) -O binary $@ build/growhome.bin
	$(SIZE) $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) -mcpu=$(MCU) -mthumb -c $< -o $@

clean:
	rm -rf build
	find . -name '*.o' -delete

.PHONY: all clean