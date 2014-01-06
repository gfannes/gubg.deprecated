#include "rinle/model/Node.hpp"
#include "gubg/Testing.hpp"

namespace 
{
	struct Data
	{
	};
	typedef rinle::model::tree::Node<Data> Node;
}

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
	TEST_TAG(main);
	auto root = Node::create();
	TEST_TRUE(root);
    Node::create(root);
    Node::create(root);
    Node::create(root);
	return 0;
}
#include "gubg/log/end.hpp"
