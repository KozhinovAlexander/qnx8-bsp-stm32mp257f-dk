## Boot parameters
define(`__LD_QNX__',            `ldqnx-64.so.2')
define(`__BOOT_ADDR__',         `0x40800000')
define(`__ARCH__',              `aarch64le')
define(`__TYPE__',              `binary')
define(`__COMPRESS_ATTR__',     `+compress')
define(`__PROCNTO_MODULES__',   `')
define(`__STARTUP__',           `startup-imx8mp-evk')
define(`__STARTUP_OPTS__',      `-u reg -W')
define(`__PROCNTO__',           `procnto-smp-instr')
define(`__PROCNTO_OPTS__',      `-mr')

## Console
define(`__CONSOLE__',           `/dev/ser1')

## ENV profile, use to overwrite the common /etc/profile
#define(`__ENV_PROFILE_FILE__',  `etc/imx8_profile')
#define(`__PROFILE_CFG__',       `/__ENV_PROFILE_FILE__ = {
#export HOME=/
#export SYSNAME=nto
#export TERM=qansi
#export PATH=/proc/boot:/sbin:/bin:/usr/bin:/usr/sbin:/usr/libexec
#export LD_LIBRARY_PATH=/proc/boot:/lib:/usr/lib:/lib/dll:/lib/dll/pci
#}')


## Audio driver

## Block driver
define(`__BLOCK_DRVR__', `devb-ram, devb-sdmmc-mx8x')

#define(`__DEVB_AHCI_DRVR__', `')
#define(`__DEVB_AHCI_OPTS__', `')
#define(`__DEVB_AHCI_DEV__', `')

#define(`__DEVB_EIDE_DRVR__', `')
#define(`__DEVB_EIDE_OPTS__', `')
#define(`__DEVB_EIDE_DEV__', `')

#define(`__DEVB_NVME_DRVR__', `')
#define(`__DEVB_NVME_OPTS__', `')
#define(`__DEVB_NVME_DEV__', `')

define(`__DEVB_RAM_DRVR__', `devb-ram')
#define(`__DEVB_RAM_OPTS__', `')
#define(`__DEVB_RAM_DEV__', `')

define(`__DEVB_SDMMC_DRVR__', `devb-sdmmc-mx8x')
#define(`__DEVB_RAM_OPTS__', `')
#define(`__DEVB_RAM_DEV__', `')

define(`__DEVB_SDMMC_START__', `
    ############################################################################################
    ## SD memory card / eMMC driver
    ############################################################################################
    ## eMMC uses USDHC3 controller
    display_msg Starting eMMC driver (/dev/emmc0)...
    devb-sdmmc-mx8x cam ifdef(`__SMMU__', `cache,quiet,bounce,smmu=on', `cache,quiet') sdmmc partitions=on blk ra=64k:2048k,memory="sysram&below4G:sysram",maxio=256,cache=4m disk name=emmc sdio idx=2,emmc,bw=8,hs=tuning_mode=standard,verbose=1
    waitfor /dev/emmc0

    ## SD card uses USDHC2
    display_msg Starting SD1 memory card driver (/dev/sd0)...
    devb-sdmmc-mx8x cam ifdef(`__SMMU__', `pnp,cache,quiet,bounce,smmu=on', `pnp,cache,quiet') blk ra=64k:2048k,memory="sysram&below4G:sysram",maxio=256,cache=4m disk name=sd sdio idx=1,bs=cd=0x30210000^12^588,verbose=1
    waitfor /dev/sd0
')

define(`__DEVB_DRVR_START__', `
__DEVB_SDMMC_START__
')

## Network driver
define(`__NET_DRVR__', `devs-ffec.so, devs-dwceqos.so')
define(`__NET_OPTS__', `-m fdt -m phy_fdt -d ffec -d dwceqos')
define(`__NET_DEV__', `ffec0 dwceqos0')

## The io-sock uses dhcpcd utility by default, uncomment the line below to use dhclient utility
#define(`__NET_DHCLIENT_SUPPORT__', `')

## USB host driver
define(`__USB_HOST_DRVR__', `devu-hcd-imx8-xhci.so')
define(`__LOCAL_XHCI_OPTS__', `-d hcd-imx8-xhci ioport=0x38100000,irq=72,ioport=0x38200000,irq=73')
define(`__USB_HOST_OPTS__', `ifdef(`__SMMU_USB_HOST_OPTS__', `__SMMU_USB_HOST_OPTS__') __LOCAL_XHCI_OPTS__')
define(`__USB_HOST_DEV__', `/dev/usb/io-usb-otg, /dev/usb/devu-hcd-imx8-xhci.so')

define(`__USB_START__', `
    ############################################################################################
    ## USB2.0 OTG1 & USB3.0 OTG2
    ############################################################################################
    display_msg "Starting OTG1 & OTG2 controller in the host mode ..."
    io-usb-otg __USB_HOST_OPTS__
foreach(`_x', `    waitfor _x', __USB_HOST_DEV__)

    display_msg "Starting USB Type-C Port Manager ..."
    tcpm-imx8mp &

    display_msg "Launching devb-umass ..."
    devb-umass cam pnp dos exe=all disk name=umass
')

## USB device driver
#define(`__USB_DEVICE_DRVR__', `')

## Persistent storge
define(`__PERSISTENT_STORAGE_DEVICE__', `/dev/sd0t179')
#define(`__PERSISTENT_STORAGE_MOUNT_POINT__', `')
#define(`__PERSISTENT_STORAGE_MOUNT_OPTS__', `')
#define(`__PERSISTENT_STORAGE_START__', `')
#define(`__PERSISTENT_STORAGE_FILES__', `')

## Serial driver
define(`__DEVC_DRVR__', `devc-sermx1')
#define(`__DEVC_OPTS__', `')
#define(`__DEVC_DEV__', `')

define(`__DEVC_START__', `
    #######################################################################
    ## Serial driver
    #######################################################################
    ## UART1 is used for the QNX console
    display_msg Starting Serial driver (/dev/ser1)...
    devc-sermx1 -e -F -S -u 1 -c 24000000 0x30890000,59
    waitfor /dev/ser1

    ## UART2 - Uncomment following three lines to initialize LPUART1
    #display_msg Starting Serial driver (/dev/ser2)...
    #devc-sermx1 -F -S -u 2 -c 24000000 0x30890000,59
    #waitfor /dev/ser2

    ## UART3 - Uncomment following three lines to initialize LPUART2
    #display_msg Starting Serial driver (/dev/ser3)...
    #devc-sermx1 -F -S -u 3 -c 24000000 0x30880000,60
    #waitfor /dev/ser3

    ## UART4 - Uncomment following three lines to initialize LPUART3
    #display_msg Starting Serial driver (/dev/ser4)...
    #devc-sermx1 -F -S -u 4 -c 24000000 0x30A60000,61
    #waitfor /dev/ser4
')

## CAN driver
define(`__CAN_DRVR__', `devcan-flexcan')

define(`__CAN_START__', `
    #######################################################################
    ## CAN driver
    #######################################################################
    display_msg Starting CAN driver (/dev/can1)...
    __CAN_DRVR__ -h -M -b250K -u1 can0
    display_msg Starting CAN driver (/dev/can2)...
    __CAN_DRVR__ -h -M -b250K -u2 can1
')

## I2C driver
define(`__I2C_DRVR__', `i2c-imx')
define(`__I2C_OPTS__', `-p 0x30A20000 -i67 -c66000000 --u 1, -p 0x30A30000 -i68 -c66000000 --u 2, -p 0x30A40000 -i69 -c66000000 --u 3')
define(`__I2C_DEV__', `/dev/i2c1, /dev/i2c2, /dev/i2c3')

#define(`__I2C_START__', `
#')

## NOR flash driver
define(`__NOR_DRVR__', `devf-nxp-fspi')

define(`__NOR_START__', `
    ###########################################################################
    ## Micron MT25QU256 driver.
    ## Quad serial NOR Flash chip can be formatted like: flashctl -p /dev/fs0p0 -efm
    ## Please note that erasing the memory will take several minutes. To see
    ## erase progress you can use verbose mode: flashctl -p /dev/fs0p0 -efmv
    ## After erasing, formatting and mounting a partition will appear as /fs0p0
    ###########################################################################
    display_msg "Starting flash driver ..."
    __NOR_DRVR__ -s soc=base=0x30BB0000:irq=139:octcomb_en=0
')

## graphics support
#ifdef(`__GRAPHICS__', `
define(`__GRAPHICS_LIB_PATH__', `/usr/lib/graphics/iMX8MP')
define(`__GRAPHICS_SCREEN_OPT__', `-c /usr/lib/graphics/iMX8MP/graphics.conf')

define(`__GRAPHICS_START__', `
    ############################################################################################
    ## Start the screen graphics
    ############################################################################################
    display_msg "Starting Screen Graphics..."
    screen -c /usr/lib/graphics/iMX8MP/graphics.conf
    waitfor /dev/screen
')

define(`__GRAPHICS_BOARD_SPECIFIC_FILES__', `
################################################################################################
## Screen Board Support i.MX8 (com.qnx.qnx710.target.screen.board.imx8)
################################################################################################
/usr/lib/libcapture-board-imx8x-nxp.so=libcapture-board-imx8x-nxp.so
/usr/lib/libcapture-soc-imx8x.so=libcapture-soc-imx8x.so

/usr/lib/graphics/iMX8MP/graphics.conf=graphics/iMX8MP/graphics.conf

/usr/lib/graphics/iMX8MP/libCLC.so=graphics/iMX8MP/libCLC.so
/usr/lib/graphics/iMX8MP/libEGL_viv.so=graphics/iMX8MP/libEGL_viv.so
/usr/lib/graphics/iMX8MP/libg2d.so=graphics/iMX8MP/libg2d.so
/usr/lib/graphics/iMX8MP/libGalcore.so=graphics/iMX8MP/libGalcore.so
/usr/lib/graphics/iMX8MP/libGAL.so=graphics/iMX8MP/libGAL.so
/usr/lib/graphics/iMX8MP/libGLESv2_viv.so=graphics/iMX8MP/libGLESv2_viv.so
/usr/lib/graphics/iMX8MP/libGLSLC.so=graphics/iMX8MP/libGLSLC.so
/usr/lib/graphics/iMX8MP/libOpenCL.so=graphics/iMX8MP/libOpenCL.so
/usr/lib/graphics/iMX8MP/libOpenVG_viv.so=graphics/iMX8MP/libOpenVG_viv.so
/usr/lib/graphics/iMX8MP/libVSC.so=graphics/iMX8MP/libVSC.so
/usr/lib/graphics/iMX8MP/libwfdcfg-imx8mp-dsi-oled.so=graphics/iMX8MP/libwfdcfg-imx8mp-dsi-oled.so
/usr/lib/graphics/iMX8MP/libwfdcfg-imx8mp-evk-it6263-adv7535.so=graphics/iMX8MP/libwfdcfg-imx8mp-evk-it6263-adv7535.so
/usr/lib/graphics/iMX8MP/libWFDimx8m.so=graphics/iMX8MP/libWFDimx8m.so
/usr/lib/graphics/iMX8MP/screen-nxp-g2d.so=graphics/iMX8MP/screen-nxp-g2d.so

#/usr/lib/graphics/iMX8MP-debug/libCLC.so=graphics/iMX8MP/libCLC.so
#/usr/lib/graphics/iMX8MP-debug/libEGL_viv.so=graphics/iMX8MP/libEGL_viv.so
#/usr/lib/graphics/iMX8MP-debug/libg2d.so=graphics/iMX8MP/libg2d.so
#/usr/lib/graphics/iMX8MP-debug/libGalcore.so=graphics/iMX8MP/libGalcore.so
#/usr/lib/graphics/iMX8MP-debug/libGAL.so=graphics/iMX8MP/libGAL.so
#/usr/lib/graphics/iMX8MP-debug/libGLESv2_viv.so=graphics/iMX8MP/libGLESv2_viv.so
#/usr/lib/graphics/iMX8MP-debug/libGLSLC.so=graphics/iMX8MP/libGLSLC.so
#/usr/lib/graphics/iMX8MP-debug/libOpenCL.so=graphics/iMX8MP/libOpenCL.so
#/usr/lib/graphics/iMX8MP-debug/libOpenVG_viv.so=graphics/iMX8MP/libOpenVG_viv.so
#/usr/lib/graphics/iMX8MP-debug/libVSC.so=graphics/iMX8MP/libVSC.so
#/usr/lib/graphics/iMX8MP-debug/libwfdcfg-imx8mp-dsi-oled.so=graphics/iMX8MP/libwfdcfg-imx8mp-dsi-oled.so
#/usr/lib/graphics/iMX8MP-debug/libwfdcfg-imx8mp-evk-it6263-adv7535.so=graphics/iMX8MP/libwfdcfg-imx8mp-evk-it6263-adv7535.so
#/usr/lib/graphics/iMX8MP-debug/libWFDimx8m.so=graphics/iMX8MP/libWFDimx8m.so
#/usr/lib/graphics/iMX8MP-debug/screen-nxp-g2d.so=graphics/iMX8MP/screen-nxp-g2d.so

/lib/dll/screen-vivante.so=screen-vivante.so

')
#')

## PCI driver
define(`__PCI_HW_DRVR__', `pci_hw-nxp-imx8m-cpu.so')
define(`__PCI_HW_MODULE__', `pci_hw-nxp-imx8m-cpu.so')
define(`__PCI_BUS_SCAN_LIMIT__', `1')
#define(`__PCI_MODULE_BLACKLIST__', `')
define(`__PCI_HW_CONFIG_FILE__', `etc/system/config/pci/pci_hw-imx8m-evk.cfg')
define(`__PCI_HW_CFG__',`
[uid=0 gid=0 perms=0444] /__PCI_HW_CONFIG_FILE__ = {
######################################  S E C T I O N  #########################################
##
[NXP_IMX8M]
##
## imx PCIe HW module configuration
## --------------------------------
##
## Disable PCIe 100MHz internal reference clock generation from MCU (available values yes or no)
INTERNAL_REF_CLOCK=no

## PERST PADs configuration
PERST_PAD_CONFIGURATION=0x30210000,7,0

## PCIe gen speed configuration
MAX_GEN_SPEED=3
}')

define(`__PCI_START__', `
    #######################################################################
    ## PCI Server
    #######################################################################
    display_msg Starting PCI Server...
    PCI_HW_MODULE=/lib/dll/pci_hw-nxp-imx8m-cpu.so
    PCI_DEBUG_MODULE=/lib/dll/pci/pci_debug2.so
    PCI_SLOG_MODULE=/lib/dll/pci/pci_slog2.so
    PCI_BKWD_COMPAT_MODULE=/lib/dll/pci/pci_bkwd_compat.so
    ## Uncomment for legacy INTx interrupt support only
    #PCI_MODULE_BLACKLIST=/lib/dll/pci/pci_cap-0x11.so:/lib/dll/pci/pci_cap-0x05.so
    PCI_HW_MODULE=/lib/dll/pci/__PCI_HW_MODULE__
    PCI_HW_CONFIG_FILE=/__PCI_HW_CONFIG_FILE__

    pci-server --bus-scan-limit=1 -c &
    waitfor /dev/pci
')

## SPI driver
define(`__IO_SPI_DRVR__', `spi-ecspi')
define(`__IO_SPI_CFG_CONTENTS__', `
[globals]
verbose=5

[bus]
busno=2
name=spi2
base=0x30830000
irq=64
input_clock=40000000

[dev]
parent_busno=2
devno=0
name=dev0
clock_rate=5000000
cpha=1
word_width=32
')

## Random
#define(`__RANDOM_DRVR__', `')
#define(`__RANDOM_DRVR_OPTS__', `')

## DMA
define(`__DMA_DRVR__', `libdma-edma.so, libdma-sdma-imx8mp1.so, libdma-sdma-imx8mp2.so, libdma-sdma-imx8mp3.so')

## RTC
define(`__RTC_DRVR__', `rtc')
define(`__RTC_OPTS__', `hw')

define(`__RTC_START__', `
    ############################################################################################
    ## RTC utility - requires i2c driver to be running
    ############################################################################################
    display_msg "Setting OS clock from RTC ..."
    __RTC_DRVR__ __RTC_OPTS__
')

## WDT kick
define(`__WDT_DRVR__', `wdtkick')
define(`__WDT_OPTS__', `-t 5000')

define(`__WDT_START__', `
    #######################################################################
    ## WatchDog utility
    ## If startup is given -W parameter then the wdtkick utility MUST
    ## be uncommented below.
    #######################################################################
    display_msg Starting Watchdog driver...
    __WDT_DRVR__ __WDT_OPTS__
')

## Customize script
define(`__CUSTOMIZE_SCRIPT_NAME__', `/scripts/board_startup.sh')
#define(`__CUSTOMIZE_SCRIPT_START__', `')
#define(`__CUSTOMIZE_SCRIPT_FILES__', `')

## Board specific files
#define(`__BOARD_EARLY_START__', `
#')

#define(`__BOARD_LATE_START__', `
#')

define(`__BOARD_FILES__', `
################################################################################################
## Messaging unit library
################################################################################################
/lib/libmu-mx8.so=libmu-mx8.so

################################################################################################
## USB Type-C Port Manager
################################################################################################
/sbin/tcpm-imx8mp=tcpm-imx8mp


################################################################################################
## END OF BUILD SCRIPT
################################################################################################
')

