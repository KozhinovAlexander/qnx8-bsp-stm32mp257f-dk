ifndef QCONFIG
QCONFIG=qconfig.mk
endif

include $(QCONFIG)

INSTALLDIR=lib/dll

# LIBS += slog2

define PINFO
PINFO DESCRIPTION=Sample io-sock module
endef

EXTRA_CLEAN+= $(PROJECT_ROOT)/mod-dwmac-stm32.use

define MODULE_SPECIFIC_OPTIONS

This can specify any module information

endef

include devs/mods.mk
