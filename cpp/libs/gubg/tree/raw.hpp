#ifndef gubg_tree_raw_hpp
#define gubg_tree_raw_hpp

//Provides a basic, policy-based type hierarchy typical for the composite pattern
//These base classes have no memory management _at all_
//A single DataPolicy has to be provided that specifies the data and its operations,
//irrespective if its a composite of leaf node

#include "gubg/tree/codes.hpp"
#include <vector>
#include <deque>

//Can go, only for debug
#include <iostream>
//#define LOG(msg) std::cout << msg << std::endl
#define LOG(msg)

namespace gubg
{
    namespace tree
    {
        namespace raw
        {
            template <typename DataPolicy>
                class Composite;
            template <typename DataPolicy>
                class Leaf;

            //These types are used later to overload the begin() and end() methods for iteration
            enum ByDataT {ByData};
            enum ByComponentT {ByComponent};
            enum CompositeOnlyT {CompositeOnly};
            enum LeafOnlyT {LeafOnly};

            //Abstract root class of the tree type hierarchy
            // * Contains a reference to its parent (pointer)
            // * Inherits from DataPolicy => the complete type hierarchy is derived from DataPolicy
            template <typename DataPolicy>
                class Component: public DataPolicy
            {
                public:
                    typedef Composite<DataPolicy> Parent;
                    typedef Leaf<DataPolicy> LeafT;
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

                public:
                    //Iterator functionality
                    //During iteration, we keep track of where we are with a stack of <node, ix>
                    // * node => The is where we are currently
                    // * ix => Indicates what branch we took. The ix of the last value is always set to 0 and indicates where we have to go next (what child we have to take)
                    typedef std::pair<Node *, size_t> PathElement;
                    typedef std::vector<PathElement> Path;
                    //We dispatch node-specific path modification to composite or leaf using this virtual function
                    virtual bool increment_(Path &) = 0;
                    //Base iteration class that iterates node per node
                    class iterator_base
                    {
                        public:
                            iterator_base(){}
                            iterator_base(Node *start):path_(1, std::make_pair(start, 0)){}
                            void operator++()
                            {
                                LOG("operator++ begin");
                                while (!path_.back().first->increment_(path_))
                                {
                                    path_.pop_back();
                                    if (path_.empty())
                                        return;
                                    ++path_.back().second;
                                }
                                LOG("operator++ end");
                            }
                            bool operator!=(const iterator_base &rhs) const {return path_ != rhs.path_;}
                            Node &operator*(){return *path_.back().first;}
                            Node *operator->(){return path_.back().first;}
//                        private:
                            Path path_;
                    };
                    //We derive different iteration styles from the basic one
                    // * The same as iterator_base
                    class iterator_by_component: public iterator_base
                    {
                        public:
                            iterator_by_component(){}
                            iterator_by_component(Node *start):iterator_base(start){}
                    };
                    // * The same as iterator_base, except that we give only out DataPolicy
                    class iterator_by_data: public iterator_base
                    {
                        public:
                            iterator_by_data(){}
                            iterator_by_data(Node *start):iterator_base(start){}
                            DataPolicy &operator*(){return *iterator_base::path_.back().first;}
                            DataPolicy *operator->(){return iterator_base::path_.back().first;}
                    };
                    // * We only stop on composite nodes
                    class iterator_composite_only: public iterator_base
                    {
                        public:
                            iterator_composite_only(){}
                            iterator_composite_only(Node *start):iterator_base(start)
                            {
                                //We proceed until we reach a composite node
                                while (!iterator_base::path_.empty() && iterator_base::path_.back().first->isLeaf())
                                    operator++();
                            }
                            void operator++()
                            {
                                iterator_base::operator++();
                                while (!iterator_base::path_.empty() && iterator_base::path_.back().first->isLeaf())
                                    iterator_base::operator++();
                            }
                            Parent &operator*(){return *dynamic_cast<Parent*>(iterator_base::path_.back().first);}
                            Parent *operator->(){return dynamic_cast<Parent*>(iterator_base::path_.back().first);}
                    };
                    // * We only stop on leaf nodes
                    class iterator_leaf_only: public iterator_base
                    {
                        public:
                            iterator_leaf_only(){}
                            iterator_leaf_only(Node *start):iterator_base(start)
                            {
                                //We proceed until we reach a leaf node
                                while (!iterator_base::path_.empty() && !iterator_base::path_.back().first->isLeaf())
                                    operator++();
                            }
                            void operator++()
                            {
                                iterator_base::operator++();
                                while (!iterator_base::path_.empty() && !iterator_base::path_.back().first->isLeaf())
                                    iterator_base::operator++();
                            }
                            LeafT &operator*(){return *dynamic_cast<LeafT*>(iterator_base::path_.back().first);}
                            LeafT *operator->(){return dynamic_cast<LeafT*>(iterator_base::path_.back().first);}
                    };
                    //A bunch of begin() and end() methods. I tried to use templates for this, but template specialization inside
                    //a template is not fun and I couldn't get it to work
                    //Anyway, by overloading on the argument, it works fine, just use e.g.: begin(CompositeOnly)
                    iterator_by_component begin(ByComponentT){return iterator_by_component(this);}
                    iterator_by_component end(ByComponentT){return iterator_by_component();}
                    iterator_by_data begin(ByDataT){return iterator_by_data(this);}
                    iterator_by_data end(ByDataT){return iterator_by_data();}
                    iterator_composite_only begin(CompositeOnlyT){return iterator_composite_only(this);}
                    iterator_composite_only end(CompositeOnlyT){return iterator_composite_only();}
                    iterator_leaf_only begin(LeafOnlyT){return iterator_leaf_only(this);}
                    iterator_leaf_only end(LeafOnlyT){return iterator_leaf_only();}
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
                    virtual bool increment_(typename ComponentT::Path &path)
                    {
                        LOG("\tComposite::increment_ " << path.size());
                        auto ix = path.back().second;
                        if (ix < childs_.size())
                        {
                            path.push_back(std::make_pair(childs_[ix], 0));
                            return true;
                        }
                        return false;
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
                    virtual bool increment_(typename ComponentT::Path &path)
                    {
                        LOG("\tLeaf::increment_ " << path.size());
                        return false;
                    }
            };
        }
    }
}

#endif
