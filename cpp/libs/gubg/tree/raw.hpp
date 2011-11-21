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
        //These types are used later to overload the begin() and end() methods for iteration
        enum ByDataT {ByData};
        enum ByNodeT {ByNode};
        enum NoLeafT {NoLeaf};
        enum LeafOnlyT {LeafOnly};

        namespace raw
        {
            template <typename DataPolicy>
                class Node: public DataPolicy
            {
                public:
                    typedef std::deque<Node*> Nodes;
                    typedef Nodes Childs;

                    Node():
                        parent_(0){}
                    virtual ~Node(){}

                    bool isLeaf() const{return childs_.empty();}

                    void setParent(Node *parent){parent_ = parent;}
                    Node *parent() const {return parent_;}

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

                    ReturnCode add(Node *component)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS_T(component, IllegalArgument);
                        component->setParent(this);
                        childs_.push_back(component);
                        MSS_END();
                    }
                    ReturnCode remove(Node *component)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS_T(component, IllegalArgument);
                        auto it = childs_.find(component);
                        MSS_T(it != childs_.end(), UnknownNode);
                        childs_.remove(it);
                        MSS_END();
                    }

                    //Returns all nodes in the tree
                    Nodes allNodes()
                    {
                        Nodes nodes;
                        addAllNodes_(nodes);
                        return nodes;
                    }

                    //Iterator functionality
                    //During iteration, we keep track of where we are with a stack of <node, ix>
                    // * node => The is where we are currently
                    // * ix => Indicates what branch we took. The ix of the last value is always set to 0 and indicates where we have to go next (what child we have to take)
                    typedef std::pair<Node *, size_t> PathElement;
                    typedef std::vector<PathElement> Path;
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
                    class iterator_by_node: public iterator_base
                    {
                        public:
                            iterator_by_node(){}
                            iterator_by_node(Node *start):iterator_base(start){}
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
                    class iterator_no_leaf: public iterator_base
                    {
                        public:
                            iterator_no_leaf(){}
                            iterator_no_leaf(Node *start):iterator_base(start)
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
                    };
                    //A bunch of begin() and end() methods. I tried to use templates for this, but template specialization inside
                    //a template is not fun and I couldn't get it to work
                    //Anyway, by overloading on the argument, it works fine, just use e.g.: begin(NoLeaf)
                    iterator_by_node begin(ByNodeT){return iterator_by_node(this);}
                    iterator_by_node end(ByNodeT){return iterator_by_node();}
                    iterator_by_data begin(ByDataT){return iterator_by_data(this);}
                    iterator_by_data end(ByDataT){return iterator_by_data();}
                    iterator_no_leaf begin(NoLeafT){return iterator_no_leaf(this);}
                    iterator_no_leaf end(NoLeafT){return iterator_no_leaf();}
                    iterator_leaf_only begin(LeafOnlyT){return iterator_leaf_only(this);}
                    iterator_leaf_only end(LeafOnlyT){return iterator_leaf_only();}

                private:
                    bool increment_(Path &path)
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
                    void addAllNodes_(Nodes &nodes)
                    {
                        nodes.push_back(this);
                        for (auto child = childs_.begin(); child != childs_.end(); ++child)
                            (*child)->addAllNodes_(nodes);
                    }

                    Node *parent_;

                    Childs childs_;
            };
        }
    }
}

#endif
