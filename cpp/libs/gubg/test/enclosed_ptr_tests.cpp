#include "catch/catch.hpp"
#include "gubg/enclosed_ptr.hpp"

namespace  { 
    class B
    {
        public:
            std::string name = "B";

        private:
            int data_b_;
    };
    class D: public B
    {
        public:
            D() {name = "D";}
        private:
            int data_d_;
    };
} 
TEST_CASE("creation", "[enclosed_ptr]")
{
    typedef gubg::enclosed_ptr<B, sizeof(D)> BPtr;
    typedef gubg::enclosed_ptr<D, sizeof(D)> DPtr;
    BPtr ptr;
    SECTION("default ctor should return nullptr") { REQUIRE(nullptr == ptr.get()); }
    SECTION("create_new for B and D")
    {
        REQUIRE(ptr.create_new<B>());
        REQUIRE(ptr.get()->name == "B");
        REQUIRE(ptr.create_new<D>());
        REQUIRE(ptr.get()->name == "D");
    }
    SECTION("")
    {
        DPtr dptr; dptr.create_new<D>();
        BPtr bptr(dptr);
    }
}
