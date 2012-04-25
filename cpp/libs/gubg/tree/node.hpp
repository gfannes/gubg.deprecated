#ifndef gubg_tree_node_hpp
#define gubg_tree_node_hpp

#include <vector>
#include <memory>

namespace gubg
{
    struct Leaf_tag {};
    struct Hub_tag {};
    template <typename Hub, typename Leaf>
        class Node
        {
            public:
                typedef std::vector<Node> Childs;

                Node(Leaf_tag): leaf(new Leaf){}
                Node(Hub_tag): hub(new Hub), childs(new Childs){}
                Node(Node &&dead)
                {
                    leaf = std::move(dead.leaf);
                    hub = std::move(dead.hub);
                    childs = std::move(dead.childs);
                }
                Node &operator=(Node &&dead)
                {
                    leaf = std::move(dead.leaf);
                    hub = std::move(dead.hub);
                    childs = std::move(dead.childs);
                    return *this;
                }

                static Node createLeaf(){return Node(Leaf_tag());}
                static Node createHub(){return Node(Hub_tag());}

                std::unique_ptr<Leaf> leaf;

                std::unique_ptr<Hub> hub;

                std::unique_ptr<Childs> childs;

            private:
                Node();
                Node(const Node &);
                Node &operator=(const Node &);
        };
}

#endif
