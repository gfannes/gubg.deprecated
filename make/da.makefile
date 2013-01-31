.PHONY: da-help
da-help:
	@echo Help:
	@echo make da-clean
	@echo make da -j

DA_CPP_FILES := \
	da \
	FixIncludeGuards CompileExe FileCache Arduino \
	build/Configuration build/Builder build/Header build/Headers build/Source build/Sources \
	package/Packages package/Local package/Boost package/GUBG package/SDL \
	compile/Compiler \
	link/Linker \

GUBG_CPP_FILES := OptionParser file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token
CPP_FILES := $(patsubst %,cpp/apps/da/%.cpp,$(DA_CPP_FILES)) $(patsubst %,cpp/libs/gubg/%.cpp,$(GUBG_CPP_FILES))
OBJECT_FILES := $(patsubst %.cpp,%.o,$(CPP_FILES))

BOOST_HEADERS := $(GUBG_BOOST)
BOOST_LIBS := $(GUBG_BOOST)/stage/lib
BOOST_LINK := -L$(BOOST_LIBS) -lboost_thread -lboost_system -lboost_regex

CPP_INCLUDE_PATHS := -Icpp/apps -Icpp/libs -I$(BOOST_HEADERS)
CPPFLAGS := -std=c++0x -O3 -pthread $(CPP_INCLUDE_PATHS)
LDFLAGS := -std=c++0x
LINK_LIBS := $(BOOST_LINK)

.PHONY: da-clean
da-clean:
	rm -f $(OBJECT_FILES)
	rm -f da

DA_CACHE := $(GUBG_TMP)/da
$(DA_CACHE):
	mkdir $@
da: $(OBJECT_FILES) $(DA_CACHE)
	g++ $(LDFLAGS) -o da $(OBJECT_FILES) $(LINK_LIBS)
	cp da $(GUBG_BIN)

%.o: %.cpp $(GUBG_BOOST)
	g++ $(CPPFLAGS) -c $< -o $@
