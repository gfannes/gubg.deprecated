#include "da/build/Configuration.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Util.hpp"
#include "gubg/string_algo.hpp"
using namespace da;
using namespace da::package;
using namespace gubg::file;
using namespace gubg;

Configuration::Configuration():
    verbose(false)
{
    forest_.add(getcwd(), {"cpp", "hpp"});

    {
        std::string str;
        if (env::expand(str, "$GUBG"))
            packages_ << GUBG(File(str));
        if (env::expand(str, "$HOME/sdks/boost"))
            packages_ << Boost(File(str));
        if (env::expand(str, "$HOME/sdks/upp"))
            packages_ << UPP(File(str));
        packages_.prune();
    }
    L("I found following packages: " << string_algo::join(packages_.names(), ", "));

    packages_.appendIncludePaths(compiler.includePaths);
    packages_.appendLibraryPaths(linker.libraryPaths);
    packages_.appendLibraries(linker.libraries);
    packages_.expandForest(forest_);
}
