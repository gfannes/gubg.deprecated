#include "catch/catch.hpp"
#include "gubg/db/KeyValue.hpp"
using namespace gubg::db;
using namespace gubg::file;

TEST_CASE("KeyValue tests", "[ut]")
{
    const File dir{"test_db"};
    remove(dir);
    REQUIRE(!isDirectory(dir));
    KeyValue kv(dir);
    REQUIRE(isDirectory(dir));
    Value value;
    const Key key = "key";
    REQUIRE(!MSS_IS_OK(kv.get(value, key)));
    REQUIRE(MSS_IS_OK(kv.set(key, "value")));
    REQUIRE(MSS_IS_OK(kv.get(value, key)));
    REQUIRE(value == "value");
}
