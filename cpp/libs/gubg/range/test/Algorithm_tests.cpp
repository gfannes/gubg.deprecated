#include "gubg/range/Algorithm.hpp"
#include "gubg/range/String.hpp"
#include "gubg/Testing.hpp"
using namespace std;
using namespace gubg::range;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
	TEST_TAG(main);
	string src("abc");
	String_char src_r(src);
	string dst;
	String_char dst_r(dst);

	TEST_FALSE(append(dst_r, src_r));
	TEST_TRUE(dst_r.empty());

	dst_r.resizeBack(src.size()-1);
	const char *front_ptr = &dst_r.front();
	TEST_FALSE(append(dst_r, src_r));
	TEST_EQ(front_ptr, &dst_r.front());

	dst_r.resizeBack(src.size());
	TEST_TRUE(append(dst_r, src_r));
	TEST_TRUE(dst_r.empty());
	TEST_EQ(src, dst);
	return 0;
}
#include "gubg/log/end.hpp"
