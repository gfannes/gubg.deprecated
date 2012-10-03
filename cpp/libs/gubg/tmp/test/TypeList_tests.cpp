#include "gubg/testing/Testing.hpp"
#include "gubg/tmp/TypeList.hpp"
#include "gubg/l.hpp"
#include <string>

int main()
{
    TEST_TAG(TypeList);
    TEST_EQ(0, gubg::tmp::TypeListSize<gubg::tmp::Null>::Value);
    typedef GUBG_TYPE_LIST(int, std::string) TL;
    TEST_EQ(2, gubg::tmp::TypeListSize<TL>::Value);
    return 0;
}
