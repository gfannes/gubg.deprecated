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
                struct iterator
                {
                    struct Element
                    {
                        const Node *node;
                        int ix;
                        bool operator==(const Element &rhs) const {return node == rhs.node && ix == rhs.ix;}
                        Element(): node(0), ix(-1){}
                        Element(const Node& n): node(&n), ix(-1){}
                    };
                    typedef std::vector<Element> Path;
                    Path path;

                    bool operator==(const iterator &rhs) const {return path == rhs.path;}
                    bool operator!=(const iterator &rhs) const {return !operator==(rhs);}
                    iterator &operator++()
                    {
                        {
                            Element& el = path.back();
                            if (!el.node->childs)
                            {
                                //We are iterating over a single leaf node
                                path.pop_back();
                                return *this;
                            }
                            auto& childs = *el.node->childs;
                            if (el.ix+1 < childs.size())
                            {
                                //We can step down by proceeding the ix
                                ++el.ix;
                                //If we end up in a hub node, we have to extend the path with a (Node*, -1)-element
                                auto& nextNode = childs[el.ix];
                                if (nextNode.hub)
                                    path.push_back(Element(nextNode));
                                return *this;
                            }
                        }
                        //We could not proceed ix, we have to step back
                        path.pop_back();
                        if (path.empty())
                            return *this;
                        return ++(*this);
                    }
                    const Node& operator*() const
                    {
                        const Element& el = path.back();
                        if (-1 == el.ix)
                            return *el.node;
                        //We are sure el.node is a hub, else el.ix has to be -1
                        return (*el.node->childs)[el.ix];
                    }
                };

                Node(Leaf_tag): leaf(new Leaf){}
                template <typename Arg>
                Node(Leaf_tag, Arg&& arg): leaf(new Leaf(std::forward<Arg>(arg))){}
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

                template <typename Arg>
                static Node createLeaf(Arg&& arg){return Node(Leaf_tag(), std::forward<Arg>(arg));}

                iterator begin() const {iterator it; it.path.push_back(typename iterator::Element(*this)); return it;}
                iterator end() const {return iterator();}

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
