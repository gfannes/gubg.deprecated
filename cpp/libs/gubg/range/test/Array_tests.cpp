#include "gubg/range/Array.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::range;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);

	typedef Array<int> Range;

	const auto BufferSize = 1024;
	int buffer[BufferSize];

	{
		TEST_TAG(ctor1);
		Range r(buffer, buffer);
		TEST_TRUE(r.empty());
	}
	{
		TEST_TAG(ctor2);
		Range r(buffer, buffer+BufferSize);
		TEST_FALSE(r.empty());
	}
	{
		TEST_TAG(forward_iteration);
		Range r(buffer, buffer+BufferSize);
		for (int i = 0; i < BufferSize; ++i)
		{
			TEST_FALSE(r.empty());
			r.popFront();
		}
		TEST_TRUE(r.empty());
	}
	{
		TEST_TAG(backward_iteration);
		Range r(buffer, buffer+BufferSize);
		for (int i = 0; i < BufferSize; ++i)
		{
			TEST_FALSE(r.empty());
			r.popBack();
		}
		TEST_TRUE(r.empty());
	}
	{
		TEST_TAG(grow_shrink_Front);
		Range r(buffer, buffer+BufferSize);
		TEST_FALSE(r.growFront(1));
		r.front() = 1234;
		r.popFront();
		r.front() = 1235;
		TEST_TRUE(r.growFront(1));
		TEST_EQ(1234, r.front());
		TEST_FALSE(r.shrinkFront(1025));
		TEST_TRUE(r.shrinkFront(1024));
		TEST_TRUE(r.empty());
		TEST_TRUE(r.growFront(1024));
		TEST_EQ(1234, r.front());
	}
	{
		TEST_TAG(grow_shrink_Back);
		Range r(buffer, buffer+BufferSize);
		TEST_FALSE(r.growBack(1));
		r.back() = 1234;
		r.popBack();
		r.back() = 1235;
		TEST_TRUE(r.growBack(1));
		TEST_EQ(1234, r.back());
		TEST_FALSE(r.shrinkBack(1025));
		TEST_TRUE(r.shrinkBack(1024));
		TEST_TRUE(r.empty());
		TEST_TRUE(r.growBack(1024));
		TEST_EQ(1234, r.back());
	}

    return 0;
}
#include "gubg/log/end.hpp"
