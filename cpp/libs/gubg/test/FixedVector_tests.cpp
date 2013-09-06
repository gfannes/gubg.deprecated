#include "gubg/Testing.hpp"
#include "gubg/FixedVector.hpp"
#include <string>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace 
{
    struct A
    {
        void f(){S();L("A");}
    };
    struct B
    {
        void f(){S();L("B");}
    };
    struct Itf
    {
        virtual void f() = 0;
    };
    template <typename T>
        struct Impl: Itf
    {
        T &t;
        Impl(T &tt):t(tt){}
        virtual void f() { t.f(); }
    };
    template <typename T>
        void wrap(void *dst, T &t)
        {
            Impl<T> impl(t);
            std::memcpy(dst, &impl, sizeof(impl));
        }
}

int main()
{
    TEST_TAG(main);

    {
        typedef FixedVector<int, 2> FV;
        FV fv;
        TEST_EQ(0, fv.size());
        fv.push_back(1);
        TEST_EQ(1, fv.size());
        fv.push_back(2);
        TEST_EQ(2, fv.size());
        fv.push_back(3);
        TEST_EQ(2, fv.size());
        fv.clear();
        TEST_EQ(0, fv.size());
        fv.push_back(1);
        fv.push_back(2);
        TEST_EQ(2, fv.size());

        auto it = fv.begin();
        TEST_EQ(1, *it);
        ++it;
        TEST_EQ(2, *it);
        ++it;
        TEST_EQ(fv.end(), it);

        TEST_EQ(1, fv[0]);
        TEST_EQ(2, fv[1]);
    }

    {
        struct S {char b[16];};
        typedef FixedVector<S, 2> FV;
        FV fv;
        fv.resize(2);
        A a;
        wrap(&fv[0], a);
        B b;
        wrap(&fv[1], b);
        ((Itf*)(&fv[0]))->f();
        ((Itf*)(&fv[1]))->f();
    }

    return 0;
}
#include "gubg/log/end.hpp"
