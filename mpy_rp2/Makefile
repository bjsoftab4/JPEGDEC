# Location of top-level MicroPython directory
MPY_DIR = ../../micropython

# Name of module
MOD = jpgdec

# Source files (.c or .py)
SRC = mpyinit.c
SRC += thumb_case.S
SRC += idiv.S idivmod.S 

# thumb_case.S

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin, rv32imc)
ARCH = armv6m

CFLAGS = -Wl,-Map=hoge.map
# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk


