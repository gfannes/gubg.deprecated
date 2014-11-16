.PHONY: fff-help
fff-help:
	@echo Help:
	@echo make clean
	@echo make fff -j

FFF_CPP_FILES := fff Tag Value Board Create Execute ToolFactory tools/Starter tools/ParseIncludes tools/ResolveHeader tools/Compiler tools/Linker tools/Runner

GUBG_CPP_FILES := OptionParser Platform Strange file/File file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token parse/Line chrono/Uptime lua/State

LUA_C_FILES := lapi lauxlib lbaselib lbitlib lcode lcorolib lctype ldblib ldebug ldo ldump lfunc lgc linit liolib llex lmathlib lmem loadlib lobject lopcodes loslib lparser lstate lstring lstrlib ltable ltablib ltm lundump lvm lzio

CPP_SRC_FILES := $(patsubst %,%.cpp,$(FFF_CPP_FILES)) $(patsubst %,../../libs/gubg/%.cpp,$(GUBG_CPP_FILES))
C_SRC_FILES := $(patsubst %,../../../c/lua-5.2.3/%.c,$(LUA_C_FILES))
CPP_OBJECT_FILES := $(patsubst %.cpp,%.cpp.obj,$(CPP_SRC_FILES))
C_OBJECT_FILES := $(patsubst %.c,%.c.obj,$(C_SRC_FILES))

ifeq ($(GUBG_PLATFORM),linux)
CPP_INCLUDE_PATHS := -I.. -I../../libs -I../../../c/lua-5.2.3
CPPFLAGS_PLATFORM :=
LDFLAGS_PLATFORM := -pthread
endif
ifeq ($(GUBG_PLATFORM),win32)
CPP_INCLUDE_PATHS := -I.. -I../../libs -I../../../c/lua-5.2.3
CPPFLAGS_PLATFORM := 
LDFLAGS_PLATFORM := 
endif
DEBUG_FLAGS :=
#DEBUG_FLAGS += -g -DGUBG_DEBUG
CPPFLAGS := -std=c++0x -O3 $(CPPFLAGS_PLATFORM) $(CPP_INCLUDE_PATHS) $(DEBUG_FLAGS)
CFLAGS := 
LDFLAGS := -std=c++0x
LINK_LIBS := $(LDFLAGS_PLATFORM)

ifeq ($(GUBG_PLATFORM),win32)
FFF_EXE := $(GUBG_BIN)/fff.exe
endif
ifeq ($(GUBG_PLATFORM),linux)
FFF_EXE := $(GUBG_BIN)/fff
endif

.PHONY: clean
clean:
	rm -f $(CPP_OBJECT_FILES)
	rm -f $(C_OBJECT_FILES)
	rm -f fff.exe

FFF_CACHE := $(GUBG_TMP)/fff
$(FFF_CACHE):
	mkdir $@
$(GUBG_BIN):
	mkdir $@
fff: $(CPP_OBJECT_FILES) $(C_OBJECT_FILES) $(FFF_CACHE) $(GUBG_BIN)
	g++ $(LDFLAGS) -o fff.exe $(CPP_OBJECT_FILES) $(C_OBJECT_FILES) $(LINK_LIBS)
	cp -p fff.exe $(FFF_EXE)

%.cpp.obj: %.cpp
	g++ -c $< -o $@ $(CPPFLAGS)
%.c.obj: %.c
	gcc -c $< -o $@ $(CFLAGS)

.PHONY: self
self:
	rm -f fff.exe
	fff fff.cpp norun 
	cp -p fff.exe $(FFF_EXE)
