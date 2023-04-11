
COMMONCXXFLAGS := -std=c++20 -O2
COMMONLDFLAGS := 

CXX := clang++
LD := clang++
CXXFLAGS := $(COMMONCXXFLAGS)
LDFLAGS := $(COMMONLDFLAGS)

WII_CXX := clang++
WII_LD := PATH=$(DEVKITPATH)/devkitPPC/bin:$(PATH) $(DEVKITPRO)/devkitPPC/bin/powerpc-eabi-g++
WII_COMMONFLAGS := -mcpu=750 -ffunction-sections -fdata-sections -DGEKKO -D__wii__ -DHW_RVL
WII_CXXFLAGS := $(COMMONCXXFLAGS) $(WII_COMMONFLAGS) --target=ppc32-none-eabi -g -nostdlibinc \
		-isystem $(DEVKITPRO)/devkitPPC/powerpc-eabi/include/c++/*/powerpc-eabi \
		-isystem $(DEVKITPRO)/devkitPPC/powerpc-eabi/include/c++/*/ \
		-isystem $(DEVKITPRO)/devkitPPC/powerpc-eabi/include \
		-I$(DEVKITPRO)/libogc/include -I$(DEVKITPRO)/portlibs/wii/include -I$(DEVKITPRO)/portlibs/ppc/include \
		-gdwarf-4
# use devkitppc ld because ld.lld dosnt suport relocation 109
WII_LDFLAGS := $(COMMONLDFLAGS) $(WII_COMMONFLAGS) -mrvl -meabi -mhard-float -g -Wl,--gc-sections -nostartfiles \
		-L$(DEVKITPRO)/libogc/lib/wii -L$(DEVKITPRO)/portlibs/wii/lib -L$(DEVKITPRO)/portlibs/ppc/lib \
		-lwiiuse -lbte -lwiikeyboard -lfat -logc -lm -lsysbase -lc -lstdc++ -Trvl.ld

COMMON_BOOT_LDFLAGS := -lSDL
BOOT_LDFLAGS := $(COMMON_BOOT_LDFLAGS) -lSDL_net
WII_BOOT_LDFLAGS := $(COMMON_BOOT_LDFLAGS) -lSDLmain

BOOT_CXXFLAGS := -Ibuild/generated/src -Isrc

COMMON_BOOT_OBJECTS := files/font main vfs/vfsUtil fontUtil
BOOT_OBJECTS := $(foreach obj,$(COMMON_BOOT_OBJECTS),build/obj/host/$(obj).o)
WII_BOOT_OBJECTS := $(foreach obj,$(COMMON_BOOT_OBJECTS),build/obj/wii/$(obj).o)

MKDIR := mkdir -p

FONT2BIN := build/tools/font2bin
EMBED := build/tools/embed

all: build host wii

host: build/host/boot
wii: build/wii/boot.elf

# folders
build:
	$(MKDIR) build
	$(MKDIR) build/tools
	$(MKDIR) build/obj
	$(MKDIR) build/obj/host
	$(MKDIR) build/obj/host/vfs
	$(MKDIR) build/obj/host/files
	$(MKDIR) build/obj/wii
	$(MKDIR) build/obj/wii/vfs
	$(MKDIR) build/obj/wii/files
	$(MKDIR) build/host
	$(MKDIR) build/wii
	$(MKDIR) build/generated
	$(MKDIR) build/generated/src
	$(MKDIR) build/generated/src/files

# tools
$(FONT2BIN): tools/font2bin.cpp
	$(CXX) $(CXXFLAGS) -Ilib/stb -o $@ $<

$(EMBED): tools/embed.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# generate font
build/generated/font.bin: resources/font.png $(FONT2BIN)
	$(FONT2BIN) $< $@

build/generated/src/files/font.hpp: build/generated/src/files/font.cpp

build/generated/src/files/font.cpp: build/generated/font.bin $(EMBED)
	$(EMBED) build/generated/src/files/font FILES_FONT files/font build/generated/font.bin font /font.bin

# create onject files
build/obj/host/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(BOOT_CXXFLAGS) -c -o $@ $<

build/obj/host/%.o: build/generated/src/%.cpp
	$(CXX) $(CXXFLAGS) $(BOOT_CXXFLAGS) -c -o $@ $<

build/obj/wii/%.o: src/%.cpp
	$(WII_CXX) $(WII_CXXFLAGS) $(BOOT_CXXFLAGS) -c -o $@ $<

build/obj/wii/%.o: build/generated/src/%.cpp
	$(WII_CXX) $(WII_CXXFLAGS) $(BOOT_CXXFLAGS) -c -o $@ $<

# link
build/host/boot: $(BOOT_OBJECTS)
	$(LD) $(LDFLAGS) $(BOOT_LDFLAGS) -o $@ $^

build/wii/boot.elf: $(WII_BOOT_OBJECTS)
	$(WII_LD) -o $@ \
		$(DEVKITPRO)/devkitPPC/powerpc-eabi/lib/crtmain.o \
		$(DEVKITPRO)/devkitPPC/lib/gcc/powerpc-eabi/*/ecrti.o \
		$(DEVKITPRO)/devkitPPC/lib/gcc/powerpc-eabi/*/ecrtn.o \
		$(WII_BOOT_LDFLAGS) $(WII_LDFLAGS) $+ -lSDL -laesnd -logc
