#ifndef gubg_tree_managed_hpp
#define gubg_tree_managed_hpp

//Provides a basic, policy-based type hierarchy typical for the composite pattern
//These base classes have memory management
//A single DataPolicy has to be provided that specifies the data and its operations,
//irrespective if its a composite of leaf node

#include "boost/shared_ptr.hpp"
#include <memory>

namespace gubg
{
    namespace tree
    {
        namespace managed
        {
            template <typename RawNode>
                struct DeleteAllNodes
                {
                    typedef RawNode* RawPtr;
                    void operator()(RawPtr root)
                    {
                        auto allNodes = root->allNodes();
                        for (auto node = allNodes.begin(); node != allNodes.end(); ++node)
                            delete *node;
                    }
                };

            template <typename RawNode>
                class Node
                {
                    public:
                        typedef Node<RawNode> Self;
                        typedef RawNode* RawPtr;
                        typedef boost::shared_ptr<RawNode> RootPtr;

                        Node():
                            node_(0){}

                        RawPtr operator->(){return node_;}

                        template <typename DerivedNode>
                            static DerivedNode down_cast(RawPtr ptr, Self &root){return DerivedNode(ptr, root);}

                        template <typename DerivedNode>
                        bool getRoot(DerivedNode &root)
                        {
                            root = down_cast<DerivedNode>(root_.get(), *this);
                            return true;
                        }

#if 0
                        class iterator_by_node
                        {
                            public:
                                iterator_by_node(){}
                                bool operator!=(const iterator_by_node &rhs) const {return it != rhs.it;}
                                void operator++(){++it;}
                                typename RawNode::iterator_by_node it;
                        };
                        iterator_by_node begin(ByNodeT){return iterator_by_node();}
                        iterator_by_node end(ByNodeT){return iterator_by_node();}
#endif

                    protected:
                        void setRoot_(RawPtr root)
                        {
                            node_ = root;
                            root_.reset(root, DeleteAllNodes<RawNode>());
                        }
                        Node(RawPtr node, Node root):
                            node_(node),
                            root_(root.root_){}

                        ReturnCode add(Node &component)
                        {
                            MSS_BEGIN(ReturnCode);
                            MSS_T(node_, InternalError);
                            MSS(node_->add(component.node_));
                            MSS_END();
                        }

                    private:
                        RawPtr node_;

                        //Each node keeps a counted reference to the root of the hierarchy
                        //If this falls to zero (all references to the root are gone), we delete
                        //all the nodes of the tree
                        RootPtr root_;
                };
        }
    }
}

#endif
