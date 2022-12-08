V=1
SOURCE_DIR=src
BUILD_DIR=build
include $(N64_INST)/include/n64.mk

all: hello.z64
.PHONY: all

OBJS = $(BUILD_DIR)/main.o

hello.z64: N64_ROM_TITLE="Hello World"

$(BUILD_DIR)/hello.elf: $(OBJS)

clean:
	rm -f $(BUILD_DIR)/* *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)