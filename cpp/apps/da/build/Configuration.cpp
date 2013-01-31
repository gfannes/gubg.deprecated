//#define GUBG_LOG
#include "da/build/Configuration.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Util.hpp"
#include "gubg/string_algo.hpp"
using namespace da;
using namespace da::package;
using namespace gubg::file;
using namespace gubg;

Configuration::Configuration()
{
    LOG_S(Configuration);
    packages_ << Local::create(File(getcwd()));

    {
        std::string str;
        if (env::expand(str, "$GUBG"))
            packages_ << GUBG::create(File(str));
        if (env::expand(str, "$GUBG_BOOST"))
            packages_ << Boost::create(File(str));
    }

    packages_ << SDL::create(File(""));

    packages_.prune();

    L("I found following packages: " << string_algo::join(packages_.names(), ", "));
}
