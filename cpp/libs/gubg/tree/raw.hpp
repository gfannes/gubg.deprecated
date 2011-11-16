#ifndef gubg_tree_raw_hpp
#define gubg_tree_raw_hpp

//Provides a basic, policy-based type hierarchy typical for the composite pattern
//These base classes have no memory management _at all_
//A single DataPolicy has to be provided that specifies the data and its operations,
//irrespective if its a composite of leaf node

#include "gubg/tree/codes.hpp"
#include <vector>
#include <deque>

namespace gubg
{
    namespace tree
    {
        namespace raw
        {
            template <typename DataPolicy>
                class Composite;

            //Abstract root class of the tree type hierarchy
            // * Contains a reference to its parent (pointer)
            // * Inherits from DataPolicy => the complete type hierarchy is derived from DataPolicy
            template <typename DataPolicy>
                class Component: public DataPolicy
            {
                public:
                    typedef Composite<DataPolicy> Parent;
                    typedef Component Node;
                    typedef std::deque<Node*> Nodes;

                    friend class Composite<DataPolicy>;

                    Component():
                        parent_(0){}
                    virtual ~Component(){}

                    virtual bool isLeaf() const = 0;

                    void setParent(Parent *parent){parent_ = parent;}
                    Parent *parent() const {return parent_;}

                    //Returns an array of nodes from this to the root of the tree
                    Nodes pathToRoot() const
                    {
                        Nodes nodes;
                        const Node *thiz = this;
                        nodes.push_back((Node *)thiz);
                        for (auto *n = parent(); n; n = n->parent())
                            nodes.push_back(n);
                        return nodes;
                    }
                    //Returns an array of nodes from the root of the tree to this
                    Nodes pathFromRoot() const
                    {
                        Nodes nodes;
                        const Node *thiz = this;
                        nodes.push_front((Node *)thiz);
                        for (auto *n = parent(); n; n = n->parent())
                            nodes.push_front(n);
                        return nodes;
                    }
                    //Returns all nodes in the tree
                    Nodes allNodes()
                    {
                        Nodes nodes;
                        addAllNodes_(nodes);
                        return nodes;
                    }

                protected:
                    virtual void addAllNodes_(Nodes &nodes) = 0;

                    Parent *parent_;
            };

            //A node of the tree that contains children
            template <typename DataPolicy>
                class Composite: public Component<DataPolicy>
            {
                public:
                    typedef Component<DataPolicy> ComponentT;
                    typedef typename ComponentT::Nodes Nodes;

                    virtual bool isLeaf() const {return false;}

                    ReturnCode add(ComponentT *component)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS_T(component, IllegalArgument);
                        component->setParent(this);
                        childs_.push_back(component);
                        MSS_END();
                    }
                    ReturnCode remove(ComponentT *component)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS_T(component, IllegalArgument);
                        auto it = childs_.find(component);
                        MSS_T(it != childs_.end(), UnknownComponent);
                        childs_.remove(it);
                        MSS_END();
                    }

                protected:
                    virtual void addAllNodes_(Nodes &nodes)
                    {
                        nodes.push_back(this);
                        for (auto child = childs_.begin(); child != childs_.end(); ++child)
                            (*child)->addAllNodes_(nodes);
                    }

                private:
                    typedef std::vector<ComponentT*> Childs;
                    Childs childs_;
            };

            template <typename DataPolicy>
                class Leaf: public Component<DataPolicy>
            {
                public:
                    typedef Component<DataPolicy> ComponentT;
                    typedef typename ComponentT::Nodes Nodes;

                    virtual bool isLeaf() const {return true;}
                protected:
                    virtual void addAllNodes_(Nodes &nodes) {nodes.push_back(this);}
            };
        }
    }
}

#endif
