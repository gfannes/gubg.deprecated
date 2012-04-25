#include "gubg/tree/node.hpp"
#include "gubg/mss.hpp"
#include <string>
using namespace std;

namespace 
{
    typedef gubg::Node<string, string> Node;
}

int main()
{
    MSS_BEGIN(int);
    Node root = Node::createHub();
    root.childs->push_back(Node::createHub());
    root.childs->push_back(Node::createLeaf());
    MSS_END();
}
