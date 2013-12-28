#include "gubg/tree/Node.hpp"
#include "gubg/Testing.hpp"
#include <string>

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
	auto root = Node::create();
	TEST_TRUE(root);
	root->data.name = "aoeaou";
	root->data.i = 42;
	addChild(root, Node::create());
	addChild(root, Node::create());
	TEST_EQ(2, root->nrChilds());
	return 0;
}
#include "gubg/log/end.hpp"
