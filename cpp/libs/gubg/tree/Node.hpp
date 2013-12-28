#ifndef HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED

#include <memory>
#include <vector>

namespace gubg { namespace tree {

	template <typename Data>
		class Node
		{
			public:
				typedef std::shared_ptr<Node<Data>> Ptr;
				typedef std::weak_ptr<Node<Data>> WPtr;
				typedef std::vector<Ptr> Childs;

				Data data;
				Childs childs;
                WPtr parent_;
                Ptr parent();

				static Ptr create(){return Ptr(new Node);}
				static Ptr create(Ptr p);
				static Ptr create(WPtr p);

				size_t nrChilds() const;

			private:
				Node(){}
		};

    template <typename Data>
        void addChild(const std::shared_ptr<Node<Data>> &parent, const std::shared_ptr<Node<Data>> &child);
    template <typename Data>
        typename Node<Data>::Ptr nextSibbling(const std::shared_ptr<Node<Data>> &node);
    template <typename Data>
        typename Node<Data>::Ptr prevSibbling(const std::shared_ptr<Node<Data>> &node);

	//Implementation
	template <typename Data>
        typename Node<Data>::Ptr Node<Data>::create(Ptr p)
        {
            auto ch = Ptr(new Node);
            addChild(p, ch);
            return ch;
        }
	template <typename Data>
        typename Node<Data>::Ptr Node<Data>::create(WPtr p)
        {
            return create(p.lock());
        }
    template <typename Data>
        typename Node<Data>::Ptr Node<Data>::parent()
        {
            return parent_.lock();
        }
    template <typename Data>
        size_t Node<Data>::nrChilds() const
        {
            return childs.size();
        }
    template <typename Data>
        void addChild(const std::shared_ptr<Node<Data>> &parent, const std::shared_ptr<Node<Data>> &child)
        {
            if (!parent || !child)
                return;
            parent->childs.push_back(child);
            child->parent_ = parent; 
        }
    template <typename Data>
        typename Node<Data>::Ptr nextSibbling(const std::shared_ptr<Node<Data>> &node)
        {
            typename Node<Data>::Ptr nothing;
            if (!node)
                return nothing;
            auto p = node->parent_.lock();
            if (!p)
                return nothing;
            auto end = p->childs.end();
            for (auto it = p->childs.begin(); it != end; ++it)
            {
                if (node == *it)
                {
                    ++it;
                    if (it != end)
                        return *it;
                    return nothing;
                }
            }
            return nothing;
        }
    template <typename Data>
        typename Node<Data>::Ptr prevSibbling(const std::shared_ptr<Node<Data>> &node)
        {
            typename Node<Data>::Ptr nothing;
            if (!node)
                return nothing;
            auto p = node->parent_.lock();
            if (!p)
                return nothing;
            auto end = p->childs.rend();
            for (auto it = p->childs.rbegin(); it != end; ++it)
            {
                if (node == *it)
                {
                    ++it;
                    if (it != end)
                        return *it;
                    return nothing;
                }
            }
            return nothing;
        }

} }

#endif
