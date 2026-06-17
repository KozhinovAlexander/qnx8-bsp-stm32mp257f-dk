# =============================================================================
# QNX 8 BSP Top-Level Makefile — STM32MP257F-DK
# =============================================================================
# Usage:
#   source /path/to/qnx800/qnxsdp-env.sh
#   make all
# =============================================================================

QARCH    := aarch64le
IFS_NAME := stm32mp257f-dk.ifs
ITS_NAME := stm32mp257f-dk.its
FIT_NAME := fitImage-qnx

# Locate mkimage (U-Boot tools)
MKIMAGE  := $(shell which mkimage 2>/dev/null || echo mkimage)

.PHONY: all startup drivers image fit clean

all: startup drivers image fit

## ── Startup ──────────────────────────────────────────────────────────────────
startup:
	$(MAKE) -C src/startup/startup-stm32mp257f-dk

## ── Drivers ──────────────────────────────────────────────────────────────────
drivers:
	$(MAKE) -C src/hardware/devc/devc-ser-stm32
	$(MAKE) -C src/hardware/i2c/i2c-stm32mp2
	$(MAKE) -C src/hardware/spi/spi-stm32mp2

## ── IFS Image ────────────────────────────────────────────────────────────────
image: startup drivers
	@echo "[mkifs] Building $(IFS_NAME)..."
	mkifs images/$(IFS_NAME:.ifs=.build) images/$(IFS_NAME)

## ── FIT Image ────────────────────────────────────────────────────────────────
fit: image
	@echo "[mkimage] Building $(FIT_NAME)..."
	@if [ ! -f images/stm32mp257f-dk.dtb ]; then \
		echo "WARNING: images/stm32mp257f-dk.dtb not found."; \
		echo "         Copy the board DTB to images/ before running 'make fit'."; \
	else \
		$(MKIMAGE) -f images/$(ITS_NAME) images/$(FIT_NAME); \
	fi

## ── Clean ────────────────────────────────────────────────────────────────────
clean:
	$(MAKE) -C src/startup/startup-stm32mp257f-dk clean
	$(MAKE) -C src/hardware/devc/devc-ser-stm32 clean
	$(MAKE) -C src/hardware/i2c/i2c-stm32mp2 clean
	$(MAKE) -C src/hardware/spi/spi-stm32mp2 clean
	rm -f images/$(IFS_NAME) images/$(FIT_NAME)
