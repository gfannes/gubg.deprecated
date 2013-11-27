.PHONY: da-help
da-help:
	@echo Help:
	@echo make da-clean
	@echo make da -j

DA_CPP_FILES := \
	da \
	FixIncludeGuards CompileExe FileCache Arduino UnitTests WebSite FixNewlines \
	build/Builder build/Header build/Headers build/Source build/Sources \
	package/Packages package/Local package/Boost package/Nana package/Arduino package/GUBG package/SDL package/DecodeIt package/Libs package/Forest \
	compile/Compiler \
	link/Linker \

GUBG_CPP_FILES := OptionParser file/File file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token parse/Line
CPP_FILES := $(patsubst %,cpp/apps/da/%.cpp,$(DA_CPP_FILES)) $(patsubst %,cpp/libs/gubg/%.cpp,$(GUBG_CPP_FILES))
OBJECT_FILES := $(patsubst %.cpp,%.da.o,$(CPP_FILES))

ifeq ($(GUBG_PLATFORM),linux)
CPP_INCLUDE_PATHS := -Icpp/apps -Icpp/libs
CPPFLAGS_PLATFORM := -pthread
endif
ifeq ($(GUBG_PLATFORM),win32)
CPP_INCLUDE_PATHS := -Icpp/apps -Icpp/libs
CPPFLAGS_PLATFORM := 
endif
#CPPFLAGS := -std=c++0x -O3 $(CPPFLAGS_PLATFORM) $(CPP_INCLUDE_PATHS) -g -DGUBG_DEBUG
CPPFLAGS := -std=c++0x -O3 $(CPPFLAGS_PLATFORM) $(CPP_INCLUDE_PATHS)
LDFLAGS := -std=c++0x
LINK_LIBS := 

.PHONY: da-clean
da-clean:
	rm -f $(OBJECT_FILES)
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
da: $(OBJECT_FILES) $(DA_CACHE) $(GUBG_BIN)
	g++ $(LDFLAGS) -o $(DA_EXE) $(OBJECT_FILES) $(LINK_LIBS)
	cp $(DA_EXE) $(GUBG_BIN)

%.da.o: %.cpp $(GUBG_BOOST)
	g++ $(CPPFLAGS) -c $< -o $@
