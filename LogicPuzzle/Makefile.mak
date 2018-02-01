PROJECT         := LogicPuzzle
DEVICES         := K64F \
                   LPC1768 \
                   NRF51_DK
GCC4MBED_DIR    := /c/gcc4mbed/
NO_FLOAT_SCANF  := 1
NO_FLOAT_PRINTF := 1

include $(GCC4MBED_DIR)/build/gcc4mbed.mk