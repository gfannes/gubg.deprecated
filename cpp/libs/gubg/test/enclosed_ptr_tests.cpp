#include "catch/catch.hpp"
#include "gubg/enclosed_ptr.hpp"

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace  { 
    class B
    {
        public:
            B(){S();L(this << " ctor " << name_);}
            B(const B &rhs){S();L(this << " cctor");}
            virtual ~B(){S();L(this << " dtor");}

            std::string name_ = "B";

            virtual int answer() const {return 0;}

        private:
            int data_b_;
    };
    typedef gubg::enclosed_ptr<B, 2*sizeof(B)> BPtr;
} 
TEST_CASE("same type", "[enclosed_ptr]")
{
    S();
    BPtr ptr;
    SECTION("default ctor should return nullptr") { REQUIRE(nullptr == ptr.get()); }
    SECTION("different sizes")
    {
        S();L("different sizes");
        typedef gubg::enclosed_ptr<B, sizeof(B)> SPtr;
        SPtr sptr; sptr.create_new<B>();
        std::cout << sptr.get()->name_ << std::endl;
        REQUIRE(!!sptr.get());
        L("before");
        ptr = sptr;
        L("after");
        std::cout << ptr.get()->name_ << std::endl;
        REQUIRE(!!ptr.get());
        REQUIRE((ptr.get()->name_ == "B"));
    }
}

namespace  { 
    class D: public B
    {
        public:
            D()
            {
                S();L(this << " ctor");
                name_ = "D";
            }
            D(const D &rhs){S();L(this << " cctor");}
            virtual ~D(){S();L(this << " dtor");}

            int answer() const override {return 42;}

        private:
            int data_d_;
    };
    typedef gubg::enclosed_ptr<D, sizeof(D)> DPtr;
} 
TEST_CASE("inherited types", "[enclosed_ptr]")
{
    S();
    typedef gubg::enclosed_ptr<D, sizeof(D)> DPtr;
    SECTION("create_new for B and D")
    {
        S();
        BPtr bptr;
        REQUIRE(bptr.create_new<B>());
        REQUIRE(bptr.get()->name_ == "B");
        REQUIRE(bptr.create_new<D>());
        REQUIRE(bptr.get()->name_ == "D");
    }
    SECTION("")
    {
        S();
        DPtr dptr; dptr.create_new<D>();
        BPtr bptr(dptr);
        REQUIRE(42 == bptr.get()->answer());
    }
}
#include "gubg/log/end.hpp"
