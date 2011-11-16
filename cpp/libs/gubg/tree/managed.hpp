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
            template <typename Data>
                struct DeleteAllNodes
                {
                    typedef gubg::tree::raw::Component<Data>* Ptr;
                    void operator()(Ptr root)
                    {
                        auto allNodes = root->allNodes();
                        for (auto node = allNodes.begin(); node != allNodes.end(); ++node)
                            delete *node;
                    }
                };

            template <typename Data>
                class Composite;

            template <typename Data>
                class Component
                {
                    public:
                        typedef gubg::tree::raw::Component<Data>* Ptr;
                        typedef boost::shared_ptr<gubg::tree::raw::Component<Data>> RootPtr;

                        friend class Composite<Data>;

                        Component():
                            node_(0){}

                        Ptr operator->(){return node_;}

                    protected:
                        Ptr node_;
                        void setRoot_(Ptr root)
                        {
                            node_ = root;
                            root_.reset(root, DeleteAllNodes<Data>());
                        }
                        Component(Ptr node, Component root):
                            node_(node),
                            root_(root.root_){}
                    private:
                        //Each node keeps a counted reference to the root of the hierarchy
                        //If this falls to zero (all references to the root are gone), we delete
                        //all the nodes of the tree
                        RootPtr root_;
                };
            template <typename Data>
                class Composite: public Component<Data>
                {
                    public:
                        typedef Component<Data> Base;
                        typedef gubg::tree::raw::Component<Data>* Ptr;
                        typedef gubg::tree::raw::Composite<Data>* This;

                        Composite(){}

                        ReturnCode add(Base &component)
                        {
                            MSS_BEGIN(ReturnCode);
                            auto t = this_();
                            MSS_T(t, InternalError);
                            MSS(t->add(component.node_));
                            MSS_END();
                        }
                    protected:

                        template <typename Node, typename Root>
                            Composite(Node node, Root root):
                                Base(node, root){}

                        This this_(){return static_cast<This>(Base::node_);}
                };
        }
    }
}

#endif
