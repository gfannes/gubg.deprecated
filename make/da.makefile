.PHONY: da-help
da-help:
	@echo Help:
	@echo make da-clean
	@echo make da -j

DA_CPP_FILES := \
	da \
	FixIncludeGuards CompileExe FileCache Arduino UnitTests WebSite FixNewlines \
	build/Builder build/Header build/Headers build/Source build/Sources \
	package/Packages package/Local package/Boost package/Nana package/Arduino package/GUBG package/SDL package/DecodeIt package/Libs package/Forest package/PkgConfig \
	compile/Compiler \
	link/Linker \

GUBG_CPP_FILES := OptionParser file/File file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token parse/Line

LUA_C_FILES := lapi lauxlib lbaselib lbitlib lcode lcorolib lctype ldblib ldebug ldo ldump lfunc lgc linit liolib llex lmathlib lmem loadlib lobject lopcodes loslib lparser lstate lstring lstrlib ltable ltablib ltm lundump lvm lzio

CPP_SRC_FILES := $(patsubst %,cpp/apps/da/%.cpp,$(DA_CPP_FILES)) $(patsubst %,cpp/libs/gubg/%.cpp,$(GUBG_CPP_FILES))
C_SRC_FILES := $(patsubst %,c/lua-5.2.3/%.c,$(LUA_C_FILES))
CPP_OBJECT_FILES := $(patsubst %.cpp,%.da.cpp.o,$(CPP_SRC_FILES))
C_OBJECT_FILES := $(patsubst %.c,%.da.c.o,$(C_SRC_FILES))

ifeq ($(GUBG_PLATFORM),linux)
CPP_INCLUDE_PATHS := -Icpp/apps -Icpp/libs
CPPFLAGS_PLATFORM := -pthread
LDFLAGS_PLATFORM := -lpthread
endif
ifeq ($(GUBG_PLATFORM),win32)
CPP_INCLUDE_PATHS := -Icpp/apps -Icpp/libs
CPPFLAGS_PLATFORM := 
LDFLAGS_PLATFORM := 
endif
DEBUG_FLAGS :=
DEBUG_FLAGS += -g -DGUBG_DEBUG
CPPFLAGS := -std=c++0x -O3 $(CPPFLAGS_PLATFORM) $(CPP_INCLUDE_PATHS) $(DEBUG_FLAGS)
CFLAGS := 
LDFLAGS := -std=c++0x $(LDFLAGS_PLATFORM)
LINK_LIBS := 

.PHONY: da-clean
da-clean:
	rm -f $(CPP_OBJECT_FILES)
	rm -f $(C_OBJECT_FILES)
	rm -f da

DA_CACHE := $(GUBG_TMP)/da
$(DA_CACHE):
	mkdir $@
$(GUBG_BIN):
	mkdir $@
ifeq ($(GUBG_PLATFORM),win32)
DA_EXE := da.exe
endif
ifeq ($(GUBG_PLATFORM),linux)
DA_EXE := da
endif
da: $(CPP_OBJECT_FILES) $(C_OBJECT_FILES) $(DA_CACHE) $(GUBG_BIN)
	g++ $(LDFLAGS) -o $(DA_EXE) $(CPP_OBJECT_FILES) $(C_OBJECT_FILES) $(LINK_LIBS)
	cp $(DA_EXE) $(GUBG_BIN)

%.da.cpp.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@
%.da.c.o: %.c
	gcc $(CFLAGS) -c $< -o $@
