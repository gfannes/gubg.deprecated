#include "gubg/Testing.hpp"
#include "gubg/tmp/TypeList.hpp"
#include <string>

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(TypeList);
    TEST_EQ(0, gubg::tmp::TypeListSize<gubg::tmp::Null>::Value);
    typedef GUBG_TYPE_LIST(int, std::string) TL;
    TEST_EQ(2, gubg::tmp::TypeListSize<TL>::Value);
    return 0;
}
#include "gubg/log/end.hpp"
