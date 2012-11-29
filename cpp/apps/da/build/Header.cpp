#include "da/build/Header.hpp"
using namespace da;

Header::Ptr Header::create(File file)
{
    return Ptr(new Header(file));
}
Header::Header(File file):
    file_(file)
{
}
