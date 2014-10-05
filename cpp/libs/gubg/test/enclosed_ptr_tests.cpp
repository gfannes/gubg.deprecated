#include "catch/catch.hpp"
#include "gubg/enclosed_ptr.hpp"

namespace  { 
    class B
    {
        public:
            std::string name_ = "B";

            virtual int answer() const {return 0;}

        private:
            int data_b_;
    };
    typedef gubg::enclosed_ptr<B, 2*sizeof(B)> BPtr;
} 
TEST_CASE("same type", "[enclosed_ptr]")
{
    BPtr ptr;
    SECTION("default ctor should return nullptr") { REQUIRE(nullptr == ptr.get()); }
    SECTION("different sizes")
    {
        typedef gubg::enclosed_ptr<B, sizeof(B)> SPtr;
        SPtr sptr; sptr.create_new<B>();
        REQUIRE(!!sptr.get());
        ptr = sptr;
        REQUIRE(!!ptr.get());
        REQUIRE(ptr.get()->name_ == "B");
    }
}

namespace  { 
    class D: public B
    {
        public:
            D() {name_ = "D";}

            int answer() const override {return 42;}

        private:
            int data_d_;
    };
    typedef gubg::enclosed_ptr<D, sizeof(D)> DPtr;
} 
TEST_CASE("inherited types", "[enclosed_ptr]")
{
    typedef gubg::enclosed_ptr<D, sizeof(D)> DPtr;
    SECTION("create_new for B and D")
    {
        BPtr bptr;
        REQUIRE(bptr.create_new<B>());
        REQUIRE(bptr.get()->name_ == "B");
        REQUIRE(bptr.create_new<D>());
        REQUIRE(bptr.get()->name_ == "D");
    }
    SECTION("")
    {
        DPtr dptr; dptr.create_new<D>();
        BPtr bptr(dptr);
        REQUIRE(42 == bptr.get()->answer());
    }
}
