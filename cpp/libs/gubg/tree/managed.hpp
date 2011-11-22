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
            template <typename T>
                T createRoot(typename T::RawPtr ptr)
                {
                    T t;
                    t.createRoot_(ptr);
                    return t;
                }

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

            template <typename RawNodeT, typename RawRootT>
                class Node
                {
                    public:
                        typedef RawNodeT RawNode;
                        typedef RawRootT RawRoot;
                        typedef Node<RawNode, RawRoot> Self;
                        typedef RawNode* RawPtr;
                        typedef boost::shared_ptr<RawRoot> RootPtr;

                        //The create template function is our friend, it needs to call createRoot_()
                        template <typename T> friend T createRoot(typename T::RawPtr);
                        //We are our own friend, we can set root_ and node_ for types based on this Node template
                        template <typename A, typename B> friend class Node;

                        Node():
                            node_(0){}

                        RawPtr operator->(){return node_;}

                    protected:
                        void createRoot_(RawPtr root)
                        {
                            node_ = root;
                            root_.reset(root, DeleteAllNodes<RawNode>());
                        }
                        template <typename T>
                        ReturnCode add_(T &t, typename T::RawPtr node)
                        {
                            MSS_BEGIN(ReturnCode);
                            MSS(node_);
                            MSS(node_->add(node));
                            t.node_ = node;
                            t.root_ = root_;
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
