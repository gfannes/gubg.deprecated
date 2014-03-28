#include "gubg/range/String.hpp"
#include "gubg/Testing.hpp"
using namespace std;
using namespace gubg::range;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
	string str;
	{
		TEST_TAG(ctor1);
        String_char r(str);
        TEST_TRUE(r.empty());
	}
	{
		TEST_TAG(ctor2);
		str = "a";
        String_char r(str);
        TEST_FALSE(r.empty());
	}
	{
		TEST_TAG(forward_iteration);
		str = "abc";
        String_char r(str);
		TEST_EQ('a', r.front());
		r.popFront();
        TEST_FALSE(r.empty());
		TEST_EQ('b', r.front());
		r.popFront();
        TEST_FALSE(r.empty());
		TEST_EQ('c', r.front());
		r.popFront();
        TEST_TRUE(r.empty());
	}
	{
		TEST_TAG(backward_iteration);
		str = "abc";
        String_char r(str);
		TEST_EQ('c', r.back());
		r.popBack();
        TEST_FALSE(r.empty());
		TEST_EQ('b', r.back());
		r.popBack();
        TEST_FALSE(r.empty());
		TEST_EQ('a', r.back());
		r.popBack();
        TEST_TRUE(r.empty());
	}
    {
		TEST_TAG(resizeBack);
		str.clear();
        String_char r(str);
        TEST_TRUE(r.resizeBack(3));
        auto sp = r;
        r.front() = 'a'; r.popFront();
        TEST_FALSE(r.empty());
        r.front() = 'b'; r.popFront();
        TEST_FALSE(r.empty());
        r.front() = 'c'; r.popFront();
        TEST_TRUE(r.empty());
		TEST_EQ(string("abc"), str);
    }
    {
		TEST_TAG(resizeFront_neg);
		str.clear();
        String_char r(str);
        TEST_FALSE(r.resizeFront(1));
    }
    {
		TEST_TAG(resizeFront_pos);
		str = "abc";
        String_char r(str);
		r.popFront(); r.popFront(); r.popFront();
        TEST_TRUE(r.empty());
        TEST_TRUE(r.resizeFront(3));
        r.front() = 'a'; r.popFront();
        TEST_FALSE(r.empty());
        r.front() = 'b'; r.popFront();
        TEST_FALSE(r.empty());
        r.front() = 'c'; r.popFront();
        TEST_TRUE(r.empty());
    }

    {
		TEST_TAG(String_cchar);
		str = "abc";
        String_cchar r(str);
        TEST_FALSE(r.empty());
        TEST_EQ('a', r.front());
        r.popFront();
        TEST_FALSE(r.empty());
        TEST_EQ('b', r.front());
        r.popFront();
        TEST_FALSE(r.empty());
        TEST_EQ('c', r.front());
        r.popFront();
        TEST_TRUE(r.empty());
    }

    return 0;
}
#include "gubg/log/end.hpp"
