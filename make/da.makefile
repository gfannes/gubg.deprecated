.PHONY: da-help
da-help:
	@echo Help:
	@echo make da-clean
	@echo make da -j

DA_CPP_FILES := \
	da \
	FixIncludeGuards CompileExe FileCache Arduino \
	build/Builder build/Header build/Headers build/Source build/Sources \
	package/Packages package/Local package/Boost package/Nana package/Arduino package/GUBG package/SDL package/DecodeIt package/Libs \
	compile/Compiler \
	link/Linker \

GUBG_CPP_FILES := OptionParser file/File file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token
CPP_FILES := $(patsubst %,cpp/apps/da/%.cpp,$(DA_CPP_FILES)) $(patsubst %,cpp/libs/gubg/%.cpp,$(GUBG_CPP_FILES))
OBJECT_FILES := $(patsubst %.cpp,%.o,$(CPP_FILES))

ifeq ($(GUBG_PLATFORM),linux)
BOOST_HEADERS := $(GUBG_BOOST)
BOOST_LIBS := $(GUBG_BOOST)/stage/lib
BOOST_LINK := -L$(BOOST_LIBS) -lboost_thread -lboost_system -lboost_regex
CPP_INCLUDE_PATHS := -Icpp/apps -Icpp/libs -I$(BOOST_HEADERS)
CPPFLAGS_PLATFORM := -pthread
endif
ifeq ($(GUBG_PLATFORM),win32)
BOOST_LINK := -lboost_thread -lboost_system -lboost_regex
CPP_INCLUDE_PATHS := -Icpp/apps -Icpp/libs
CPPFLAGS_PLATFORM := 
endif
CPPFLAGS := -std=c++0x -O3 $(CPPFLAGS_PLATFORM) $(CPP_INCLUDE_PATHS) -g -DGUBG_DEBUG
#CPPFLAGS := -std=c++0x -O3 $(CPPFLAGS_PLATFORM) $(CPP_INCLUDE_PATHS)
LDFLAGS := -std=c++0x
LINK_LIBS := $(BOOST_LINK)

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

%.o: %.cpp $(GUBG_BOOST)
	g++ $(CPPFLAGS) -c $< -o $@
