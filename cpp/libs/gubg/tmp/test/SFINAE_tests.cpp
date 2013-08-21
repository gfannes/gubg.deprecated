#include "gubg/Testing.hpp"
#include "gubg/tmp/Types.hpp"
#include <utility>
#include "gubg/tmp/SFINAE.hpp"
#include "gubg/tmp/Equal.hpp"
using namespace gubg::tmp;

class HasFoo
{
    public:
int X;
    private:
};
class HasNotFoo
{
    public:
    private:
};

template<typename T>
class DetectX
{
    struct Fallback { int X; }; // add member name "X"
    struct Derived : T, Fallback { };
 
    template<typename U, U> struct Check;
 
    typedef char ArrayOfOne[1];  // typedef for an array of size one.
    typedef char ArrayOfTwo[2];  // typedef for an array of size two.
 
    template<typename U> 
    static ArrayOfOne & func(Check<int Fallback::*, &U::X> *);
 
    template<typename U> 
    static ArrayOfTwo & func(...);
 
  public:
    typedef DetectX type;
    enum { value = sizeof(func<Derived>(0)) == 2 };
};

bool eval(bool (*f)()) { return (*f)(); }

bool eval(void (*f)()) { (*f)(); return true; }

template <typename Type>
bool eval(Type* obj, bool (Type::*method)()) { return (obj->*method)(); }

template <typename Type>
bool eval(Type* obj, void (Type::*method)()) { (obj->*method)(); return true; }

namespace 
{
    struct HasUsedMemory
    {
        size_t used_memory() const
        {
            return 42;
        }
    };
    struct HasNotUsedMemory
    {
    };
}

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(SFINAE);
    TEST_TRUE(DetectX<HasFoo>::value);
    TEST_FALSE(DetectX<HasNotFoo>::value);
    TEST_TRUE((Equal<HasUsedMemoryMethod<HasUsedMemory>::Value, HasMethod>::Value));
    TEST_TRUE((Equal<HasUsedMemoryMethod<HasNotUsedMemory>::Value, HasNotMethod>::Value));
    return 0;
}
#include "gubg/log/end.hpp"
