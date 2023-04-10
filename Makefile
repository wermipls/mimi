V=1
SOURCE_DIR=src
BUILD_DIR=build
include $(N64_INST)/include/n64.mk
ROM_VERSION=$(shell git describe --always --match 'NOT A TAG')
NAME=mimi-$(ROM_VERSION)
ROMNAME=$(NAME).z64
REPO_URL=github.com/wermipls/mimi
FS=$(BUILD_DIR)/data.dfs

N64_CFLAGS += -DROM_VERSION=\""$(ROM_VERSION)"\"
N64_CFLAGS += -DREPO_URL=\""$(REPO_URL)"\"

all: $(ROMNAME)
.PHONY: all

build/main.o: .FORCE
.FORCE:

SRCS = $(wildcard $(SOURCE_DIR)/*.c)
OBJS = $(SRCS:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)

$(ROMNAME): N64_ROM_TITLE="mimi controller test"
$(ROMNAME): $(FS)

$(FS): $(wildcard fs/*) $(wildcard gfx/*)
	mkdir -p fs/gfx/
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_0.png fs/gfx/stick_0.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_1.png fs/gfx/stick_1.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_2.png fs/gfx/stick_2.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_3.png fs/gfx/stick_3.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_4.png fs/gfx/stick_4.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_5.png fs/gfx/stick_5.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_6.png fs/gfx/stick_6.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_7.png fs/gfx/stick_7.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/stick_neutral.png fs/gfx/stick_neutral.sprite
	$(N64_ROOTDIR)/bin/mksprite 32 gfx/point.png fs/gfx/point.sprite
	$(N64_MKDFS) $@ fs

$(BUILD_DIR)/$(NAME).elf: $(OBJS)

clean:
	rm -f $(BUILD_DIR)/* *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
