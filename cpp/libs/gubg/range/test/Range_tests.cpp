#include "gubg/range/Range.hpp"
#include "gubg/Testing.hpp"
#include <string>

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
template <typename Container>
struct Shared
{
	typedef std::shared_ptr<Container> Ptr;
	Ptr ptr;
	Shared(): ptr(new Container) {}
	Container &ref() {return *ptr;}
	const Container &ref() const {return *ptr;}
};

struct Index_Length
{
	size_t ix;
	size_t len;

	template <typename Container>
		Index_Length(const Container &container): ix(0), len(container.size()) {}

	bool empty() const {return len == 0;}
};
typedef gubg::range::Range<int, Shared<std::string>, Index_Length> Range;

int main()
{
	TEST_TAG(main);
	Range r;
	TEST_TRUE(r.empty());
	r.ref() = "abc";
	TEST_FALSE(r.empty());
	return 0;
}
#include "gubg/log/end.hpp"
