FLASHFILE ?= $(BINFILE)

# -e: erase
STM32LOADER_FLAGS += -p $(PROG_DEV) -e -w $(FLASHFILE)

FLASHER ?= $(RIOTTOOLS)/stm32loader/stm32loader.py
FFLAGS ?= $(STM32LOADER_FLAGS)
