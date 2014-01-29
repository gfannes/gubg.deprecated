#include "gubg/tree/Node.hpp"
#include "gubg/Testing.hpp"
#include <string>
#include <vector>

namespace 
{
	struct Data
	{
		std::string name;
		int i;
		Data(): i(0) {}
	};
}

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
	TEST_TAG(main);

	typedef gubg::tree::Node<Data> Node;
	Node n;
	TEST_FALSE(n);

	n = Node::create();
	TEST_TRUE(n);
	TEST_EQ(0, n.nrChilds());

	n.data().name = "Hello";
	n.data().i = 42;

	{
		const auto cn = n;
		TEST_EQ(42, cn.data().i);
	}

	{
		TEST_TAG(get);
		auto ch = n.firstChild();
		TEST_FALSE(ch);
	}

	{
		TEST_TAG(push);
		n.pushChild(Node::create());
		TEST_EQ(1, n.nrChilds());
		n.pushChild(Node::create());
		TEST_EQ(2, n.nrChilds());
	}

	{
		TEST_TAG(forward);
		auto ch = n.firstChild();
		TEST_TRUE(ch);
		TEST_FALSE(ch.prevSibling());
		ch = ch.nextSibling();
		TEST_TRUE(ch);
		TEST_TRUE(ch.prevSibling());
		TEST_EQ(n.lastChild(), ch);
		ch = ch.nextSibling();
		TEST_FALSE(ch);
	}
	{
		TEST_TAG(backward);
		auto ch = n.lastChild();
		TEST_TRUE(ch);
		TEST_FALSE(ch.nextSibling());
		ch = ch.prevSibling();
		TEST_TRUE(ch);
		TEST_TRUE(ch.nextSibling());
		TEST_EQ(n.firstChild(), ch);
		ch = ch.prevSibling();
		TEST_FALSE(ch);
	}

	{
		TEST_TAG(shift);
		auto ch = n.shiftChild();
		TEST_EQ(1, n.nrChilds());
		TEST_TRUE(ch);
		ch = n.shiftChild();
		TEST_EQ(0, n.nrChilds());
		TEST_TRUE(ch);
		ch = n.shiftChild();
		TEST_EQ(0, n.nrChilds());
		TEST_FALSE(ch);
	}

	{
		TEST_TAG(stress);
		for (int i = 0; i < 10000000; ++i)
			n.pushChild(Node::create());
        L("Node " << sizeof(Node));
        L("shared_ptr" << sizeof(std::shared_ptr<int>));
        L("Data " << sizeof(Data));
        L("string " << sizeof(std::string));
        L("tree::impl<Data> " << sizeof(gubg::tree::impl::Node<Data>));
        L("mutex " << sizeof(std::mutex));
	}
    //getc(stdin);

	return 0;
}
#include "gubg/log/end.hpp"
