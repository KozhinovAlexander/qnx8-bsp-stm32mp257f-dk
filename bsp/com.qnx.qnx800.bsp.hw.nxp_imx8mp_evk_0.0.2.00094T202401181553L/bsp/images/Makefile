ifndef QCONFIG
QCONFIG=qconfig.mk
endif

# IPL bootable image is created under Linux OS only.
# For other OS IPL binary is created only and mingw utility is needed for creating imx bootable image
# (see UserGuide documantation).
IPL_IMAGE :=
ifeq ($(OS),Windows_NT)
	MKIMAGE_TOOL := mkimage_imx8.exe
else
	MKIMAGE_TOOL := mkimage_imx8
	ifeq ($(shell uname -s),Linux)
		IPL_IMAGE := ipl-imx
	endif
endif

include $(QCONFIG)

HOST_MKIFS := mkifs
HOST_SED := sed
SUFFIXES := .build .ifs

BUILD_TEMPLATE = $(CURDIR)/../../Templates_build_sdp800
INSTALL=../install
BOARD=imx8mp-evk

IPL_START_ADDR := 0x40200000
IFS_LOAD_ADDR := 0x48000000

.PHONY: all clean $(IPL_IMAGE)

all: qnx-ifs qnx-ifs-graphics ipl

# To boot from U-boot "go" command:
#  - load ifs-imx8mp-evk IFS image to 0x40800000
#  - load imx8mp-evk.dtb to 0x40000000
#  - run "go 0x40800000 0x40000000"
ifs-$(BOARD): $(BOARD).build
	$(CP_HOST) $(BOARD).build $(BOARD)-go.build
	$(HOST_SED) -i 's/u reg/u arg/' $(BOARD)-go.build
	$(HOST_SED) -i 's/,binary/,raw/' $(BOARD)-go.build
	# remove memory allocated for ATF from system
	$(HOST_SED) -i '/startup-imx8mp-evk/ s/$$/ -r 0x56000000,0x2000000,1/' $(BOARD)-go.build
	$(HOST_MKIFS) -v -r$(INSTALL) $(MKIFSFLAGS)  $(BOARD)-go.build $@
	$(RM_HOST) $(BOARD)-go.build

clean:
	$(RM_HOST) qnx-ifs* *.bin *.imx
	$(RM_HOST) *.sym

qnx-ifs: $(BOARD).build
	$(HOST_MKIFS) -vvv -r $(INSTALL) $(MKIFSFLAGS) $^ $@

qnx-ifs-graphics:  $(BOARD)-graphics.build
	$(HOST_MKIFS) -r $(INSTALL) $(MKIFSFLAGS) $^ $@

disk_image: qnx-ifs
	$(SHELL) ./mkdiskimage

ipl:
	${QNX_HOST}/usr/bin/ntoaarch64-objcopy --input-target=elf64-littleaarch64 --output-target=binary ../install/aarch64le/boot/sys/ipl-$(BOARD)-spl ./ipl-$(BOARD)-spl.bin
	${QNX_HOST}/usr/bin/ntoaarch64-objcopy --input-target=elf64-littleaarch64 --output-target=binary ../install/aarch64le/boot/sys/ipl-$(BOARD) ./ipl-$(BOARD).bin
	./tools/imx-mkimage/$(MKIMAGE_TOOL) -version v2 -fit -loader ipl-$(BOARD)-spl.bin 0x920000 -second_loader ipl-$(BOARD).bin $(IPL_START_ADDR) 0x60000 -out ipl-$(BOARD).imx
	./tools/imx-mkimage/$(MKIMAGE_TOOL) -version v2 -dev flexspi -fit -loader ipl-$(BOARD)-spl.bin 0x920000 -second_loader ipl-$(BOARD).bin $(IPL_START_ADDR) 0x60000 -out ipl-$(BOARD)_flash.imx
	#
	# Uncomment for add "containter" to QNX-IFS image (used for HAB authentication)
	#./tools/imx-mkimage/$(MKIMAGE_TOOL) -version v2 -fit -loader qnx-ifs $(IFS_LOAD_ADDR) -out qnx-ifs.imx

	$(RM_HOST) ipl-$(BOARD).bin ipl-$(BOARD)-spl_flash.bin ipl-$(BOARD).bin

-include $(BUILD_TEMPLATE)/template.mk

