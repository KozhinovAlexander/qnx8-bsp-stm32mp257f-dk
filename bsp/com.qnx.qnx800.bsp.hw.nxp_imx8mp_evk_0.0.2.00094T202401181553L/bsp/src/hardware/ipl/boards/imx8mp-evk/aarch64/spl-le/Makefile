include ../../../common.mk
ASFLAGS += -DIMX_SPL_BOOT -D__ASM__
CCFLAGS += -DIMX_SPL_BOOT
LNKCCFLAGS = -DIMX_SPL_BOOT

LDF_ := $(filter-out -T$(PROJECT_ROOT)/$(SECTION)/$(SECTION).lnk,$(LDF_))
LDF_qcc := $(filter-out -M -nostartup -Wl,-T$(PROJECT_ROOT)/$(SECTION)/$(SECTION).lnk,$(LDF_qcc))
LDF_ = -nostartfiles -T$(PROJECT_ROOT)/$(SECTION)/$(SECTION)-spl.lnk -static
LDF_qcc = -M -nostartup -Wl,-T$(PROJECT_ROOT)/$(SECTION)/$(SECTION)-spl.lnk -static
