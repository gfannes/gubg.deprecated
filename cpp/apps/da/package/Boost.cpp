#include "da/package/Boost.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace da::package;
using namespace gubg::file;

Boost::Boost(){}
Boost::Boost(const File &base):
    base_(base)
{
}

bool Boost::exists() const
{
    return gubg::file::exists(base_);
}
