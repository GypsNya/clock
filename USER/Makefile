#TARGET name
TARGET = CLOCK

#build dir
BUILD_DIR = build

#TARGET dir
TARGET_DIR = target

#c version
CPP_VER = c99

#CPU arch info
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

C_DEFS = \
-DSTM32F429xx\
-DUSE_HAL_DRIVER

AS_DEFS = 

#debug switch
DEBUG = 0
#optimized
OPTIMIZED = 0

#librarys dir, if have no, empty this value
LIBDIR = 

#asm source
ASM_SOURCE = #
#../LIB/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f429xx.S
ASM_DEPENDENCY = \
../LIB/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c \
 \

#asmm source
ASMM_SOURCES = \
../LIB/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f429xx.S \
 \

#include
C_INCLUDE = \
-I../LIB/CMSIS/Include \
-I../LIB/Drivers/STM32F4xx_HAL_Driver/Inc \
-I../LIB/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
-Idevice \
-Ifatfs \
-Ifsmc \
-Ilcd \
-Iled \
-Ipic \
-Ircc \
-Irtc \
-Isd \
-Isystick \
-Iusart \
-I. \
-I tiny_ttf \

#source
C_LIB_SOURCE = \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_sdmmc.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sd.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c \
../LIB/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
 \
  

C_USER_SOURCE = \
$(wildcard *.c) \
device/bsp_geek_429.c \
$(wildcard fatfs/*.c) \
fsmc/bsp_fsmc.c \
led/bsp_led.c \
lcd/bsp_lcd.c \
$(wildcard pic/*.c) \
rcc/bsp_rcc.c \
rtc/bsp_rtc.c \
$(wildcard sd/*.c) \
systick/bsp_systick.c\
usart/bsp_usart.c \
tiny_ttf/dis_ttf_font.c \

########################################
#ttf
C_USER_TTF_SOURCE = \


TTF_INCLUDE += $(C_INCLUDE)
########################################

LD_SCRIPT = CHIP_CFG/STM32F429IGTx_FLASH.ld

#gcc version
PREFIX = arm-none-eabi-
#gcc location
ifdef GCC_PATH:
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif

#output hex and binary
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

OPT = 

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDE) $(OPT) -std=c11 -Wall -fdata-sections -ffunction-sections
TTF_CFLAGS += $(MCU) $(C_DEFS) $(TTF_INCLUDE) $(OPT) -std=c11 -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwraf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

LIBS = -lc -lm -lnosys
LDFLAGS = $(MCU) -specs=nano.specs -T$(LD_SCRIPT) $(LIBDIR) -Wl,--start-group $(LIBS) -Wl,--end-group -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

C_USER_SOURCE += $(C_LIB_SOURCE)
C_USER_SOURCE += $(ASM_DEPENDENCY)

all: $(TARGET_DIR)/$(TARGET).elf $(TARGET_DIR)/$(TARGET).hex $(TARGET_DIR)/$(TARGET).bin

OBJECT += $(addprefix $(BUILD_DIR)/, $(notdir $(C_USER_SOURCE:.c=.o)))
vpath %.c $(sort $(dir $(C_USER_SOURCE)))
OBJECT += $(addprefix $(BUILD_DIR)/, $(notdir $(ASM_SOURCE:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCE)))
OBJECT += $(addprefix $(BUILD_DIR)/,$(notdir $(ASMM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.list)) $< -o $@
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(TARGET_DIR)/$(TARGET).elf: $(TTF_OBJECT) $(OBJECT) Makefile | $(TARGET_DIR)
	$(CC) $(OBJECT) $(TTF_OBJECT) $(LDFLAGS) -o $@
	$(SZ) $@

$(TARGET_DIR)/%.hex: $(TARGET_DIR)/%.elf | $(TARGET_DIR)
	$(HEX) $< $@
$(TARGET_DIR)/%.bin: $(TARGET_DIR)/%.elf | $(TARGET_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@		

$(TARGET_DIR):
	mkdir $@		

clean:
	-del /q $(BUILD_DIR)
	-del /q $(TARGET_DIR)
-include $(wildcard $(BUILD_DIR)/*.d)

#################################
#opencd
#################################
#E:/APP/xpack-openocd-0.12.0-6/openocd/scripts/interface/stlink-v2-1.cfg
INTERFACE_CFG=CHIP_CFG/stlink-v2-1.cfg
#E:/APP/xpack-openocd-0.12.0-6/openocd/scripts/target/stm32f4x.cfg
TARGET_CFG=CHIP_CFG/stm32f4x.cfg

download:
	-openocd -f ${INTERFACE_CFG} -f ${TARGET_CFG} -c init -c halt -c "flash write_image erase ${TARGET_DIR}/${TARGET}.bin 0x8000000" -c reset -c shutdown

debug:
	-openocd -f ${INTERFACE_CFG} -f ${TARGET_CFG}
