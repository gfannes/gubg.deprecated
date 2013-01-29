#include "da/package/Boost.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/logging/Log.hpp"
using namespace da::package;
using namespace gubg::file;

Boost::Boost(const File &base):
    base_(base),
    libDir_(base)
{
    libDir_ << "stage/lib";
    linkSettings_.libraries.insert("boost_thread");
    linkSettings_.libraries.insert("boost_system");
    linkSettings_.libraries.insert("boost_filesystem");
    linkSettings_.libraries.insert("boost_regex");
    linkSettings_.libraries.insert("boost_signals");
}

bool Boost::exists() const
{
    LOG_S(exists, base_.name());
    return gubg::file::exists(base_);
}
