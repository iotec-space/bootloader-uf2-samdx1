BOARD=zero
BOARD_DIR ?= boards/$(BOARD)
-include Makefile.user
include $(BOARD_DIR)/board.mk
CC=arm-none-eabi-gcc
ifeq ($(CHIP_FAMILY), samd21)
COMMON_FLAGS = -mthumb -mcpu=cortex-m0plus -Os -g -DSAMD21
endif
ifeq ($(CHIP_FAMILY), samd51)
COMMON_FLAGS = -mthumb -mcpu=cortex-m4 -Og -g -DSAMD51
endif
WFLAGS = \
-Werror -Wall -Wstrict-prototypes \
-Werror-implicit-function-declaration -Wpointer-arith -std=gnu99 \
-ffunction-sections -fdata-sections -Wchar-subscripts -Wcomment -Wformat=2 \
-Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch \
-Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wno-undef \
-Wbad-function-cast -Wwrite-strings -Waggregate-return \
-Wformat -Wmissing-format-attribute \
-Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs \
-Wlong-long -Wunreachable-code -Wcast-align \
-Wno-missing-braces -Wno-overflow -Wno-shadow -Wno-attributes -Wno-packed -Wno-pointer-sign
CFLAGS = $(COMMON_FLAGS) \
-x c -c -pipe -nostdlib \
--param max-inline-insns-single=500 \
-fno-strict-aliasing -fdata-sections -ffunction-sections \
-D__$(CHIP_VARIANT)__ \
$(WFLAGS)

UF2_VERSION_BASE = $(shell git describe --dirty --always --tags)

ifeq ($(CHIP_FAMILY), samd21)
LINKER_SCRIPT=scripts/samd21j18a.ld
BOOTLOADER_SIZE=8192
APP_START=8192
SELF_LINKER_SCRIPT=scripts/samd21j18a_self.ld
endif

ifeq ($(CHIP_FAMILY), samd51)
LINKER_SCRIPT=scripts/samd51j19a.ld
BOOTLOADER_SIZE=16384
APP_START=16384
SELF_LINKER_SCRIPT=scripts/samd51j19a_self.ld
endif

LDFLAGS= $(COMMON_FLAGS) \
-Wall -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common \
-Wl,--warn-section-align \
-save-temps -nostartfiles \
--specs=nano.specs --specs=nosys.specs
BUILD_PATH=build/$(BOARD)
INCLUDES = -I. -I./inc -I./inc/preprocessor
INCLUDES += -I$(BOARD_DIR) -Ilib/cmsis/CMSIS/Include -Ilib/usb_msc
INCLUDES += -I$(BUILD_PATH)


ifeq ($(CHIP_FAMILY), samd21)
INCLUDES += -Ilib/samd21/samd21a/include/
endif

ifeq ($(CHIP_FAMILY), samd51)
ifeq ($(findstring SAME51,$(CHIP_VARIANT)),SAME51)
INCLUDES += -Ilib/same51/include/
else
ifeq ($(findstring SAME54,$(CHIP_VARIANT)),SAME54)
INCLUDES += -Ilib/same54/include/
else
INCLUDES += -Ilib/samd51/include/
endif
endif
endif

COMMON_SRC = \
	src/flash_$(CHIP_FAMILY).c \
	src/init_$(CHIP_FAMILY).c \
	src/startup_$(CHIP_FAMILY).c \
	src/usart_sam_ba.c \
	src/screen.c \
	src/images.c \
	src/utils.c

SOURCES = $(COMMON_SRC) \
	src/cdc_enumerate.c \
	src/fat.c \
	src/main.c \
	src/msc.c \
	src/sam_ba_monitor.c \
	src/uart_driver.c \
	src/hid.c \

SELF_SOURCES = $(COMMON_SRC) \
	src/selfmain.c

OBJECTS = $(patsubst src/%.c,$(BUILD_PATH)/%.o,$(SOURCES))
SELF_OBJECTS = $(patsubst src/%.c,$(BUILD_PATH)/%.o,$(SELF_SOURCES)) $(BUILD_PATH)/selfdata.o


ifeq ($(WITH_MCUBOOT),1)
WFLAGS += -Wno-long-long
WFLAGS += -Wno-redundant-decls 
WFLAGS += -Wno-nested-externs

CFLAGS += -DWITH_MCUBOOT
BOOTLOADER_SIZE=32768
APP_START=65536


INCLUDES += -I$(BOARD_DIR)/mcuboot

MCUBOOT_BOARD_SOURCES += $(BOARD_DIR)/mcuboot/flash_area.c
MCUBOOT_BOARD_SOURCES += $(BOARD_DIR)/mcuboot/flash_samx_mcuboot.c

MCUBOOT_OBJECTS += $(patsubst $(BOARD_DIR)/mcuboot/%.c,$(BUILD_PATH)/mcuboot/%.o,$(MCUBOOT_BOARD_SOURCES))

INCLUDES += -Ilib/mcuboot/ext/mbedtls-asn1/include
INCLUDES += -Ilib/mcuboot/boot/bootutil/include

MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/boot_record.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/bootutil_misc.c
#MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/bootutil_misc.h
#MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/bootutil_priv.h
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/bootutil_public.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/caps.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/encrypted.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/fault_injection_hardening.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/image_ecdsa.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/image_ed25519.c
#MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/image_rsa.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/image_validate.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/loader.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/swap_misc.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/swap_move.c
#MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/swap_priv.h
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/swap_scratch.c
MCUBOOT_LIB_SOURCES += lib/mcuboot/boot/bootutil/src/tlv.c

MCUBOOT_OBJECTS += $(patsubst lib/mcuboot/boot/bootutil/src/%.c,$(BUILD_PATH)/mcuboot/lib/%.o,$(MCUBOOT_LIB_SOURCES))

ifeq ($(WITH_MCUBOOT_TINYCRYPT),1)
INCLUDES += -Ilib/mcuboot/ext/tinycrypt/lib/include
CFLAGS_MCUBOOT += -DMCUBOOT_USE_TINYCRYPT

#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/aes_decrypt.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/aes_encrypt.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/cbc_mode.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/ccm_mode.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/cmac_mode.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/ctr_mode.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/ctr_prng.c
MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/ecc.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/ecc_dh.c
MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/ecc_dsa.c
MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/ecc_platform_specific.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/hmac.c
#MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/hmac_prng.c
MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/sha256.c
MCUBOOT_TINYCRYPT_SOURCES += lib/mcuboot/ext/tinycrypt/lib/source/utils.c

MCUBOOT_OBJECTS += $(patsubst lib/mcuboot/ext/tinycrypt/lib/source/%.c,$(BUILD_PATH)/mcuboot/tinycrypt/%.o,$(MCUBOOT_TINYCRYPT_SOURCES))

$(BUILD_PATH)/mcuboot/tinycrypt/%.o: lib/mcuboot/ext/tinycrypt/lib/source/%.c $(wildcard inc/*.h boards/*/*.h) $(BUILD_PATH)/uf2_version.h
	mkdir -p $(BUILD_PATH)/mcuboot/tinycrypt
	$(CC) $(CFLAGS) $(CFLAGS_MCUBOOT) $(BLD_EXTA_FLAGS) $(INCLUDES) $(MCUBOOT_INCLUDES) $< -o $@

endif

OBJECTS += $(MCUBOOT_OBJECTS)

$(BUILD_PATH)/mcuboot/%.o: $(BOARD_DIR)/mcuboot/%.c $(wildcard inc/*.h boards/**/*.h) $(BUILD_PATH)/uf2_version.h
	mkdir -p $(BUILD_PATH)/mcuboot
	$(CC) $(CFLAGS) $(CFLAGS_MCUBOOT) $(BLD_EXTA_FLAGS) $(INCLUDES) $(MCUBOOT_INCLUDES) $< -o $@

$(BUILD_PATH)/mcuboot/lib/%.o: lib/mcuboot/boot/bootutil/src/%.c $(wildcard inc/*.h boards/**/*.h) $(BUILD_PATH)/uf2_version.h
	mkdir -p $(BUILD_PATH)/mcuboot/lib
	$(CC) $(CFLAGS) $(CFLAGS_MCUBOOT) $(BLD_EXTA_FLAGS) $(INCLUDES) $(MCUBOOT_INCLUDES) $< -o $@
endif


NAME=bootloader-$(BOARD)-$(UF2_VERSION_BASE)
EXECUTABLE=$(BUILD_PATH)/$(NAME).bin
SELF_EXECUTABLE=$(BUILD_PATH)/update-$(NAME).uf2
SELF_EXECUTABLE_INO=$(BUILD_PATH)/update-$(NAME).ino
EXECUTABLE_LATEST=$(BUILD_PATH)/bootloader-$(BOARD)-latest

SUBMODULES = lib/uf2/README.md

all: $(SUBMODULES) dirs $(EXECUTABLE) $(SELF_EXECUTABLE)

r: run
b: burn
l: logs

burn: all
	node scripts/dbgtool.js fuses
	node scripts/dbgtool.js $(BUILD_PATH)/$(NAME).bin

run: burn wait logs

# This currently only works on macOS with a BMP debugger attached.
# It's meant to flash the bootloader in a loop.
BMP = $(shell ls -1 /dev/cu.usbmodem* | head -1)
BMP_ARGS = --nx -ex "set mem inaccessible-by-default off" -ex "set confirm off" -ex "target extended-remote $(BMP)" -ex "mon tpwr enable" -ex "mon swdp_scan" -ex "attach 1"
GDB = arm-none-eabi-gdb

bmp-flash: $(BUILD_PATH)/$(NAME).bin
	@test "X$(BMP)" != "X"
	$(GDB) $(BMP_ARGS) -ex "load" -ex "quit" $(BUILD_PATH)/$(NAME).elf | tee build/flash.log
	@grep -q "Transfer rate" build/flash.log

bmp-flashone:
	while : ; do $(MAKE) bmp-flash && exit 0 ; sleep 1 ; done
	afplay /System/Library/PrivateFrameworks/ScreenReader.framework/Versions/A/Resources/Sounds/Error.aiff

bmp-loop:
	while : ; do $(MAKE) bmp-flashone ; sleep 5 ; done

bmp-gdb: $(BUILD_PATH)/$(NAME).bin
	$(GDB) $(BMP_ARGS) $(BUILD_PATH)/$(NAME).elf

$(BUILD_PATH)/flash.jlink: $(BUILD_PATH)/$(NAME).bin
	echo " \n\
r \n\
h \n\
loadbin \"$(BUILD_PATH)/$(NAME).bin\", 0x0 \n\
verifybin \"$(BUILD_PATH)/$(NAME).bin\", 0x0 \n\
r \n\
qc \n\
" > $(BUILD_PATH)/flash.jlink

jlink-flash: $(BUILD_PATH)/$(NAME).bin $(BUILD_PATH)/flash.jlink
	JLinkExe -if swd -device AT$(CHIP_VARIANT) -speed 4000 -CommanderScript $(BUILD_PATH)/flash.jlink

wait:
	sleep 5

logs:
	node scripts/dbgtool.js $(BUILD_PATH)/$(NAME).map

selflogs:
	node scripts/dbgtool.js $(BUILD_PATH)/update-$(NAME).map

dirs:
	@echo "Building $(BOARD)"
	-@mkdir -p $(BUILD_PATH)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -L$(BUILD_PATH) $(LDFLAGS) \
		 -T$(LINKER_SCRIPT) \
		 -Wl,-Map,$(BUILD_PATH)/$(NAME).map -o $(BUILD_PATH)/$(NAME).elf $(OBJECTS)
	arm-none-eabi-objcopy -O binary $(BUILD_PATH)/$(NAME).elf $@
	@echo
	-@arm-none-eabi-size $(BUILD_PATH)/$(NAME).elf | awk '{ s=$$1+$$2; print } END { print ""; print "Space left: " ($(BOOTLOADER_SIZE)-s) }'
	@echo
	ln -fs $(NAME).bin $(EXECUTABLE_LATEST).bin  # For flashing
	ln -fs $(NAME).elf $(EXECUTABLE_LATEST).elf  # For debugging

$(BUILD_PATH)/uf2_version.h: Makefile
	echo "#define UF2_VERSION_BASE \"$(UF2_VERSION_BASE)\""> $@

$(SELF_EXECUTABLE): $(SELF_OBJECTS)
	$(CC) -L$(BUILD_PATH) $(LDFLAGS) \
		 -T$(SELF_LINKER_SCRIPT) \
		 -Wl,-Map,$(BUILD_PATH)/update-$(NAME).map -o $(BUILD_PATH)/update-$(NAME).elf $(SELF_OBJECTS)
	arm-none-eabi-objcopy -O binary $(BUILD_PATH)/update-$(NAME).elf $(BUILD_PATH)/update-$(NAME).bin
	python3 lib/uf2/utils/uf2conv.py -b $(APP_START) -c -o $@ $(BUILD_PATH)/update-$(NAME).bin

$(BUILD_PATH)/%.o: src/%.c $(wildcard inc/*.h boards/*/*.h) $(BUILD_PATH)/uf2_version.h
	echo "$<"
	$(CC) $(CFLAGS) $(BLD_EXTA_FLAGS) $(INCLUDES) $< -o $@

$(BUILD_PATH)/%.o: $(BUILD_PATH)/%.c
	$(CC) $(CFLAGS) $(BLD_EXTA_FLAGS) $(INCLUDES) $< -o $@

$(BUILD_PATH)/selfdata.c: $(EXECUTABLE) scripts/gendata.py src/sketch.cpp
	python3 scripts/gendata.py $(APP_START) $(EXECUTABLE)

clean:
	rm -rf build

gdb:
	arm-none-eabi-gdb $(BUILD_PATH)/$(NAME).elf

tui:
	arm-none-eabi-gdb -tui $(BUILD_PATH)/$(NAME).elf

%.asmdump: %.o
	arm-none-eabi-objdump -d $< > $@

applet0: $(BUILD_PATH)/flash.asmdump
	node scripts/genapplet.js $< flash_write

applet1: $(BUILD_PATH)/utils.asmdump
	node scripts/genapplet.js $< resetIntoApp

drop-board: all
	@echo "*** Copy files for $(BOARD)"
	mkdir -p build/drop
	rm -rf build/drop/$(BOARD)
	mkdir -p build/drop/$(BOARD)
	cp $(SELF_EXECUTABLE) build/drop/$(BOARD)/
	cp $(EXECUTABLE) build/drop/$(BOARD)/
# .ino works only for SAMD21 right now; suppress for SAMD51
ifeq ($(CHIP_FAMILY),samd21)
	cp $(SELF_EXECUTABLE_INO) build/drop/$(BOARD)/
	cp $(BOARD_DIR)/board_config.h build/drop/$(BOARD)/
endif

drop-pkg:
	mv build/drop build/uf2-samd21-$(UF2_VERSION_BASE)
	cp bin-README.md build/uf2-samd21-$(UF2_VERSION_BASE)/README.md
	cd build; 7z a uf2-samd21-$(UF2_VERSION_BASE).zip uf2-samd21-$(UF2_VERSION_BASE)
	rm -rf build/uf2-samd21-$(UF2_VERSION_BASE)

all-boards:
	for f in `cd boards; ls` ; do "$(MAKE)" BOARD=$$f drop-board || break -1; done

drop: all-boards drop-pkg

$(SUBMODULES):
	git submodule update --init --recursive
