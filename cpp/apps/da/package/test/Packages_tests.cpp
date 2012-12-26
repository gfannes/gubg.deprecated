#define GUBG_LOG
#include "gubg/Testing.hpp"
#include "da/package/Packages.hpp"
#include "gubg/env/Util.hpp"
using namespace da::package;
using namespace gubg::file;
using namespace gubg;

int main()
{
    TEST_TAG(main);
    TEST_TRUE(Package::Ptr() == Package::Ptr());

    Packages pks;
    pks << Boost(File("/boost/is/not/present/here"));
    std::string str;
    if (env::expand(str, "$HOME/sdks/boost"))
    {
        pks << Boost(File(str));
        pks << Boost(File(str));
        pks << Boost(File(str));
    }
    TEST_EQ(4, pks.size());
    pks.prune();
    TEST_EQ(1, pks.size());
    return 0;
}
