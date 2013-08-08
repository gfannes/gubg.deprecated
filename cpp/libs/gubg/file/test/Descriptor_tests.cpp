#include "gubg/file/Descriptor.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::file;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    auto d = Descriptor::listen(1234);
    auto dd = Descriptor::listen(1234);
    Descriptor c;
    d.accept(c);
    return 0;
}
#include "gubg/log/end.hpp"
