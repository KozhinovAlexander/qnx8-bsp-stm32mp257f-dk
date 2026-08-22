# =============================================================================
# QNX 8 BSP Top-Level Makefile — STM32MP257F-DK
# =============================================================================
# Usage:
#   source /path/to/qnx800/qnxsdp-env.sh
#   make all
# =============================================================================

CURRENT_DIR := $(CURDIR)
BUILD_DIR := $(CURRENT_DIR)/build
SHELL := /bin/bash

DT_STM32MP2_PATH := $(CURRENT_DIR)/dt-stm32mp

CROSS_COMPILE := aarch64-linux-gnu-
CROSS_COMPILE32 := arm-linux-gnueabihf-

ARCH     := arm
ARCH64   := aarch64
QARCH    := aarch64le
PLATFORM := stm32mp2
SOC	  	 := $(PLATFORM)57f
BOARD 	 := $(SOC)-dk
STM32_FLAVOR := a35-td
BOOT_CHAIN := optee

EXT_DT_STM32MP2_A32_TD_PATH := $(DT_STM32MP2_PATH)/$(PLATFORM)/$(STM32_FLAVOR)

IFS_NAME := $(BOARD).ifs
ITS_NAME := $(BOARD).its
FIT_NAME := fitImage-qnx

UBOOT_DEFCONFIG := $(PLATFORM)5_defconfig
UBOOT_BUILD_DIR := $(BUILD_DIR)/u-boot

OPTEE_BUILD_DIR := $(BUILD_DIR)/optee_os

ARM_TF_A_BUILD_DIR := $(BUILD_DIR)/arm-trusted-firmware

STM32MP_DDR_FW_PATH := $(CURRENT_DIR)/stm32-ddr-phy-binary/$(PLATFORM)

PYTHON3 ?= $(shell which python3)

STM32_Programmer_CLI_DIR ?= $(HOME)/STMicroelectronics/STM32Cube/STM32CubeProgrammer
STM32_Programmer_CLI := $(STM32_Programmer_CLI_DIR)/bin/STM32_Programmer_CLI

TSV_DIR := $(CURRENT_DIR)/deploy/images/flashlayout_$(BOARD)
FLASH_LAYOUT_NAME := FlashLayout_board_$(BOARD)-qnx
FLASH_LAYOUT := $(TSV_DIR)/$(FLASH_LAYOUT_NAME)

QNX_INSTALL_DIR := $(HOME)/qnx800
BSP_ROOT_DIR := $(CURRENT_DIR)/bsp/qnx800.bsp.hw.st_$(BOARD)/bsp/BSP_st-$(BOARD)_be-800

.PHONY: all
all: bsp_all

# At least on Ubuntu 24.04, in combination with the apparmor package,
# the Ubuntu kernel now restricts the use of unprivileged user namespaces.
# This affects all programs on the system that are unprivileged and unconfined.
# You can disable this restriction by running
.PHONY: fix_app_armor
fix_app_armor:
	@echo 0 | sudo tee /proc/sys/kernel/apparmor_restrict_unprivileged_userns

.PHONY: install_dependencies
install_dependencies:
	@sudo apt purge gcc-multilib -y
	@sudo apt update
	@sudo apt install -y gawk wget git git-lfs diffstat unzip texinfo \
		chrpath socat cpio python3 python3-pip python3-pexpect \
		xz-utils debianutils iputils-ping python3-git python3-jinja2 libsdl1.2-dev\
		pylint xterm bsdmainutils libusb-1.0-0 bison flex libssl-dev libgmp-dev libmpc-dev \
		lz4 zstd libegl1-mesa-dev coreutils bsdmainutils sed curl bc lrzsz corkscrew cvs mercurial \
		nfs-common nfs-kernel-server libarchive-zip-perl dos2unix texi2html libxml2-utils \
		python3-pyelftools gcc-15 gcc-15-aarch64-linux-gnu device-tree-compiler dfu-util \
		android-tools-fastboot tftpd-hpa cloud-guest-utils
	@sudo apt upgrade -y
	@sudo apt autoremove -y
	@sudo apt clean
	$(MAKE) install_arm_cross_compiler

# see: https://learn.arm.com/install-guides/gcc/cross/
#	   https://learn.arm.com/install-guides/gcc/arm-gnu
.PHONY: install_arm_cross_compiler
install_arm_cross_compiler:
	@sudo apt update
	@sudo apt install gcc-arm-linux-gnueabihf -y
	@sudo apt install gcc-aarch64-linux-gnu -y
	@aarch64-linux-gnu-gcc --version
	@arm-linux-gnueabihf-gcc --version

.PHONY: git_submodules_configure
git_submodules_configure:
	@declare -A modules_map=( \
		["u-boot"]="https://github.com/STMicroelectronics/u-boot" \
		["stm32-ddr-phy-binary"]="https://github.com/STMicroelectronics/stm32-ddr-phy-binary.git" \
		["arm-trusted-firmware"]="https://github.com/STMicroelectronics/arm-trusted-firmware.git" \
		["dt-stm32mp"]="https://github.com/STMicroelectronics/dt-stm32mp.git" \
		["optee_os"]="https://github.com/STMicroelectronics/optee_os.git" \
		["stm32-ddr-phy-binary"]="https://github.com/STMicroelectronics/stm32-ddr-phy-binary.git" \
	); \
	for key in "$${!modules_map[@]}"; do \
		value="$${modules_map[$$key]}"; \
		pushd $(CURRENT_DIR); \
		git submodule add --force --name $$key $$value $$key; \
		popd; \
	done; \
	git submodule update --init --recursive

setup_host_tftp_server:
	@sudo apt install tftpd-hpa -y
	@sudo systemctl enable tftpd-hpa
	@sudo systemctl start tftpd-hpa
	@TFTP_DIRECTORY=$$(sed -n 's/^TFTP_DIRECTORY[[:space:]]*=[[:space:]]*"\([^"]*\)".*/\1/p' /etc/default/tftpd-hpa); \
		echo "TFTP directory: $$TFTP_DIRECTORY"; \
		sudo chown -R $(shell id -u):$(shell id -g) $$TFTP_DIRECTORY;
	@echo "TFTP server setup completed. Please ensure the TFTP root directory is configured correctly."

tftp_server_transfer:
	@TFTP_DIRECTORY=$$(sed -n 's/^TFTP_DIRECTORY[[:space:]]*=[[:space:]]*"\([^"]*\)".*/\1/p' /etc/default/tftpd-hpa); \
		echo "Transferring files to TFTP directory: $$TFTP_DIRECTORY"; \
		rm -rf $$TFTP_DIRECTORY/$(FILE); \
		cp -r $(FILE) $$TFTP_DIRECTORY;

.PHONY: uboot_build
uboot_build:
	$(MAKE) -j$(nproc) -C u-boot KBUILD_OUTPUT=$(UBOOT_BUILD_DIR) \
		DEVICE_TREE=$(BOARD) CROSS_COMPILE=$(CROSS_COMPILE) \
		$(UBOOT_DEFCONFIG)
	$(MAKE) -j$(nproc) -C u-boot KBUILD_OUTPUT=$(UBOOT_BUILD_DIR) \
		DEVICE_TREE=$(BOARD) CROSS_COMPILE=$(CROSS_COMPILE) \
		all

.PHONY: uboot_clean
uboot_clean:
	$(MAKE) -C u-boot KBUILD_OUTPUT=$(UBOOT_BUILD_DIR) clean
	@rm -rf $(UBOOT_BUILD_DIR)

# U-Boot is BL33 in the TF-A boot flow
.PHONY: uboot
uboot: uboot_build

# optee build:
# 	https://wiki.st.com/stm32mpu/wiki/How_to_build_OP-TEE_components
# 	https://trustedfirmware-a.readthedocs.io/en/latest/plat/st/stm32mp2.html#stm32mp2
# ostl is OpenSTLinux config. use: $(BOARD)-ca35tdcid-ostl.dts
# default: CFG_EMBED_DTB_SOURCE_FILE=$(BOARD).dts
.PHONY: optee_build
optee_build:
	$(MAKE) -j$(nproc) PLATFORM=$(PLATFORM) CROSS_COMPILE_core=$(CROSS_COMPILE) \
		ARCH=arm CFG_ARM64_core=y CROSS_COMPILE_ta_arm64=$(CROSS_COMPILE) \
		NOWERROR=1 LDFLAGS= CFG_TEE_CORE_LOG_LEVEL=2 CFG_TEE_CORE_DEBUG=y \
		PYTHON3=$(PYTHON3) \
		-C $(CURRENT_DIR)/optee_os O=$(OPTEE_BUILD_DIR) \
		CFG_EMBED_DTB_SOURCE_FILE=$(BOARD).dts \
		CFG_EXT_DTS=$(EXT_DT_STM32MP2_A32_TD_PATH)/optee \
		CFG_STM32MP_PROFILE=system_services

.PHONY: optee_clean
optee_clean:
	$(MAKE) -j$(nproc) -C optee_os O=$(OPTEE_BUILD_DIR) clean
	@rm -rf $(OPTEE_BUILD_DIR)

.PHONY: optee
optee: optee_build

# arm trusted firmware build:
# 	https://trustedfirmware-a.readthedocs.io/en/latest/plat/st/stm32mp2.html#stm32mp2
#	https://wiki.st.com/stm32mpu/wiki/How_to_configure_TF-A_FIP
# 	for trusted boot build configuration see: https://wiki.st.com/stm32mpu/wiki/TF-A_BL2_Trusted_Board_Boot
.PHONY: tf-a_build
tf-a_build: uboot_build optee_build
	$(MAKE) -j$(nproc) PLAT=$(PLATFORM) ARCH=$(ARCH64) ARM_ARCH_MAJOR=8 CROSS_COMPILE=$(CROSS_COMPILE) \
		DEBUG=1 LOG_LEVEL=40 -C $(CURRENT_DIR)/arm-trusted-firmware \
		BUILD_PLAT=$(ARM_TF_A_BUILD_DIR) \
		DTB_FILE_NAME=$(BOARD).dtb SPD=opteed \
		TFA_EXTERNAL_DT=$(EXT_DT_STM32MP2_A32_TD_PATH)/tf-a \
		PSA_FWU_SUPPORT=1 STM32MP_SDMMC=1 STM32MP25=1 \
		dtbs
	$(MAKE) -j$(nproc) PLAT=$(PLATFORM) ARCH=$(ARCH64) ARM_ARCH_MAJOR=8 CROSS_COMPILE=$(CROSS_COMPILE) \
		DEBUG=1 LOG_LEVEL=40 -C $(CURRENT_DIR)/arm-trusted-firmware \
		BUILD_PLAT=$(ARM_TF_A_BUILD_DIR) \
		DTB_FILE_NAME=$(BOARD).dtb SPD=opteed \
		TFA_EXTERNAL_DT=$(EXT_DT_STM32MP2_A32_TD_PATH)/tf-a \
		PSA_FWU_SUPPORT=1 STM32MP_SDMMC=1 STM32MP25=1 STM32MP_LPDDR4_TYPE=1 \
		STM32MP_DDR_FW_PATH=$(STM32MP_DDR_FW_PATH) \
		BL33=$(UBOOT_BUILD_DIR)/u-boot-nodtb.bin \
		BL33_CFG=$(UBOOT_BUILD_DIR)/u-boot.dtb \
		BL32=$(OPTEE_BUILD_DIR)/core/tee-header_v2.bin \
		BL32_EXTRA1=$(OPTEE_BUILD_DIR)/core/tee-pager_v2.bin \
		BL32_EXTRA2=$(OPTEE_BUILD_DIR)/core/tee-pageable_v2.bin \
		all
	$(CURRENT_DIR)/arm-trusted-firmware/tools/fiptool \
		--verbose update \
		--nt-fw u-boot-nodtb.bin \
		--hw-config $(UBOOT_BUILD_DIR)/u-boot.dtb fip-stm32mp257f-dk.bin

.PHONY: tf-a_clean
tf-a_clean:
	$(MAKE) -j$(nproc) -C $(CURRENT_DIR)/arm-trusted-firmware O=$(ARM_TF_A_BUILD_DIR) \
		PLAT=$(PLATFORM) \
		clean
	@rm -rf $(ARM_TF_A_BUILD_DIR)

.PHONY: tf-a
tf-a: tf-a_build

.PHONY: clean
clean: uboot_clean optee_clean tf-a_clean bsp_clean
	@rm -rf $(BUILD_DIR)

.PHONY: configure_stm32_programmer
configure_stm32_programmer:
	@sudo cp $(STM32_Programmer_CLI_DIR)/Drivers/rules/*.rules /etc/udev/rules.d/
	@sudo udevadm control --reload-rules
	@sudo udevadm trigger

# NOTE: Accoring to my experience - it needs to be run few times until all the partitions from .tsv file
# are succesfully flashed.
.PHONY: flash_stm32_programmer
flash_stm32_programmer: configure_stm32_programmer
	@set -e; \
	dev_idx=$$($(STM32_Programmer_CLI) -l usb | grep -o 'USB[0-9]\+'); \
	echo "Flashing device $$dev_idx with $(FLASH_LAYOUT).tsv"; \
	$(STM32_Programmer_CLI) -c port=$$dev_idx -d $(FLASH_LAYOUT).tsv; \
	$(STM32_Programmer_CLI) -c port=$$dev_idx -detach;

# Use SDCARD argument to pass /dev/sdX device path to flash the SD card
.PHONY: sdcard_provision
sdcard_provision:
	@test -n "$(SD_CARD_DEV)" || { echo "Usage: make sdcard_provision SD_CARD_DEV=/dev/sdX"; exit 1; }
	@echo "Provisioning SD card $(SD_CARD_DEV) with $(FLASH_LAYOUT).tsv"
	@$(TSV_DIR)/scripts/create_sdcard_from_flashlayout.sh $(FLASH_LAYOUT).tsv
	@sudo wipefs -a $(SD_CARD_DEV)
	@sudo dd if=$(TSV_DIR)/flashlayout_$(BOARD)_$(FLASH_LAYOUT_NAME).raw \
		of=$(SD_CARD_DEV) bs=8M conv=fdatasync status=progress
	@test -n "$(SD_CARD_DEV)" || { echo "Usage: make grow_part SD_CARD_DEV=/dev/sdX"; exit 1; }
	@last_part=$$(sudo parted -s $(SD_CARD_DEV) print | awk '/^ *[0-9]+/ {n=$$1} END {print n}'); \
		echo "Last partition detected: $(SD_CARD_DEV)$$last_part"; \
		sudo growpart $(SD_CARD_DEV) $$last_part || true; \
		sudo mkfs.ext4 -L userfs $(SD_CARD_DEV)$$last_part; \
		pre_last_part=$$(($$last_part - 1)); \
		echo "Prev to Last partition detected: $(SD_CARD_DEV)$$pre_last_part"; \
		sudo mkfs.vfat -F 32 $(SD_CARD_DEV)$$pre_last_part;
	@$(MAKE) copy_boot_part_content SD_CARD_DEV=$(SD_CARD_DEV)

copy_boot_part_content:
	@echo "Copying boot partition content to SD card"; \
		last_part=$$(sudo parted -s $(SD_CARD_DEV) print | awk '/^ *[0-9]+/ {n=$$1} END {print n}'); \
		echo "Last partition detected: $(SD_CARD_DEV)$$last_part"; \
		pre_last_part=$$(($$last_part - 1)); \
		echo "Prev to Last partition detected: $(SD_CARD_DEV)$$pre_last_part"; \
		sudo mount $(SD_CARD_DEV)$$pre_last_part /mnt; \
		sudo cp -r $(CURRENT_DIR)/deploy/images/flashlayout_$(BOARD)/boot_part/* /mnt/; \
		sudo umount /mnt; \
		sudo mount -a;

# please refer to: https://www.qnx.com/developers/docs/BSP8.0/com.qnx.doc.bsp_raspberrypi.bcm2712.rpi5_8.0/topic/common/build_commandline.html
.PHONY: bsp_all
bsp_all:
	@mkdir -p $(BSP_ROOT_DIR)/install
	@source $(QNX_INSTALL_DIR)/qnxsdp-env.sh \
		&& $(MAKE) JLEVEL=$$(nproc) -C$(BSP_ROOT_DIR)/images clean \
		&& $(MAKE) JLEVEL=$$(nproc) -C$(BSP_ROOT_DIR) LIST=CONTROL $(MAKE_LIST_EXCLUDE) all \
		&& $(MAKE) JLEVEL=$$(nproc) -C$(BSP_ROOT_DIR)/images $(MAKE_LIST_EXCLUDE) ifs-$(BOARD).raw
	@$(MAKE) bsp_prebuilt
	@$(MAKE) tftp_server_transfer FILE=$(BSP_ROOT_DIR)/images/ifs-$(BOARD).raw

bsp_prebuilt:
	@mkdir -p $(BSP_ROOT_DIR)/prebuilt
	@cp -r $(BSP_ROOT_DIR)/install/* $(BSP_ROOT_DIR)/prebuilt

.PHONY: bsp_clean
bsp_clean:
	@source $(QNX_INSTALL_DIR)/qnxsdp-env.sh \
		&& $(MAKE) JLEVEL=$$(nproc) -C $(BSP_ROOT_DIR)/images clean \
		&& $(MAKE) JLEVEL=$$(nproc) -C $(BSP_ROOT_DIR) clean
