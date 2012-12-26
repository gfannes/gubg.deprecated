#include "da/package/Packages.hpp"
using namespace da::package;
using namespace gubg::file;
using namespace gubg;

int main()
{
    Packages pks;
    pks << Boost(File(env::expand("$HOME/sdks/boost")));

    return 0;
}
