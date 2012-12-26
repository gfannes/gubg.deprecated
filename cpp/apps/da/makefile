.PHONY: help
help:
	@echo Help:
	@echo make clean
	@echo make build -j

.PHONY: build
build: da

DA_CPP_FILES := \
	da \
	FixIncludeGuards CompileExe \
	build/Configuration build/Builder build/Header build/Headers build/Source build/Sources \
	package/Packages package/Boost package/GUBG \
	compile/Compiler \
	link/Linker \

GUBG_CPP_FILES := OptionParser file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token
CPP_FILES := $(patsubst %,%.cpp,$(DA_CPP_FILES)) $(patsubst %,../../libs/gubg/%.cpp,$(GUBG_CPP_FILES))
OBJECT_FILES := $(patsubst %.cpp,%.o,$(CPP_FILES))

BOOST := $(HOME)/sdks/boost
BOOST_HEADERS := $(BOOST)
BOOST_LIBS := $(BOOST)/stage/lib
.PHONY: boost
boost: $(BOOST_HEADERS) $(BOOST_LIBS)
$(BOOST_HEADERS):
	$(error You have to specify where the boost headers can be found)
$(BOOST_LIBS):
	$(error You have to specify where the boost libraries can be found)
BOOST_LINK := -L$(BOOST_LIBS) -lboost_thread -lboost_system -lboost_regex

CPP_INCLUDE_PATHS := -I.. -I../../libs -I$(BOOST_HEADERS)
CPPFLAGS := -std=c++0x -O3 -pthread $(CPP_INCLUDE_PATHS)
LDFLAGS := -std=c++0x
LINK_LIBS := $(BOOST_LINK)

.PHONY: clean
clean:
	rm -f $(OBJECT_FILES)
	rm -f da

da: $(OBJECT_FILES)
	g++ $(LDFLAGS) -o da $(OBJECT_FILES) $(LINK_LIBS)

%.o: %.cpp $(BOOST)
	g++ $(CPPFLAGS) -c $< -o $@
