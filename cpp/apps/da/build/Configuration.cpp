#include "da/build/Configuration.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace da;
using namespace gubg::file;

Configuration::Configuration():
    verbose(false)
{
    compiler.includePaths.push_back(File("/home/gfannes/gubg/cpp/libs"));
    compiler.includePaths.push_back(File("/home/gfannes/gubg/cpp/apps"));
    compiler.includePaths.push_back(File("/home/gfannes/sdks/boost"));
    
    linker.libraryPaths.push_back(File("/home/gfannes/sdks/boost/stage/lib"));
    linker.libraries.push_back("boost_thread");
    linker.libraries.push_back("boost_system");
    linker.libraries.push_back("boost_filesystem");
    linker.libraries.push_back("boost_regex");
    linker.libraries.push_back("boost_signals");

    forest_.add(getcwd(), {"cpp", "hpp"});
    forest_.add(File("/home/gfannes/gubg/cpp/libs/gubg"), {"cpp", "hpp"});
}
