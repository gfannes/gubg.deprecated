#ifndef HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED

#include <memory>

namespace gubg { namespace tree {

#if defined(L_TEMPLATE) || defined(L_SELF) || defined(L_TEMPLATE_SCOPE)
#error Helper macros are already in use
#endif

#define L_TEMPLATE template <typename Data, template <typename T, typename A> class Container>
#define L_SELF Node<Data, Container>
#define L_TEMPLATE_SCOPE L_TEMPLATE auto L_SELF

	L_TEMPLATE
		class Node;

	//We need this as a free function because we have to set a weak_ptr from child to parent
	L_TEMPLATE
		void addChild(const std::shared_ptr<L_SELF> &parent, const std::shared_ptr<L_SELF> &child);
	L_TEMPLATE
		auto nextSibbling(const std::shared_ptr<L_SELF> &node) -> typename L_SELF::Ptr;
	L_TEMPLATE
		auto prevSibbling(const std::shared_ptr<L_SELF> &node) -> typename L_SELF::Ptr;

	L_TEMPLATE
		class Node
		{
			public:
				typedef L_SELF Self;
				typedef std::shared_ptr<Self> Ptr;
				typedef std::weak_ptr<Self> WPtr;
				typedef Container<Ptr, std::allocator<Ptr>> Childs;

				Data data;
				Childs childs;
				Ptr parent();

				static Ptr create();
				static Ptr create(Ptr p);
				static Ptr create(WPtr p);

				size_t nrChilds() const;

			private:
				Node(){}

				friend void addChild<Data, Container>(const std::shared_ptr<L_SELF> &, const std::shared_ptr<L_SELF> &);
				WPtr parent_;
		};

	//Implementation of methods
	L_TEMPLATE_SCOPE::create() -> Ptr
	{
		return Ptr(new Node);
	}
	L_TEMPLATE_SCOPE::create(L_SELF::Ptr p) -> Ptr
	{
		auto ch = create();
		addChild(p, ch);
		return ch;
	}
	L_TEMPLATE_SCOPE::create(L_SELF::WPtr p) -> Ptr
	{
		return create(p.lock());
	}
	L_TEMPLATE_SCOPE::parent() -> Ptr
	{
		return parent_.lock();
	}
	L_TEMPLATE_SCOPE::nrChilds() const -> size_t
	{
		return childs.size();
	}

	//Implementation of free functions
	L_TEMPLATE
		void addChild(const std::shared_ptr<L_SELF> &parent, const std::shared_ptr<L_SELF> &child)
		{
			if (!parent || !child)
				return;
			parent->childs.push_back(child);
			child->parent_ = parent; 
		}
	L_TEMPLATE
		auto nextSibbling(const std::shared_ptr<L_SELF> &node) -> typename L_SELF::Ptr
		{
			typename L_SELF::Ptr nothing;
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
	L_TEMPLATE
		auto prevSibbling(const std::shared_ptr<L_SELF> &node) -> typename L_SELF::Ptr
		{
			typename L_SELF::Ptr nothing;
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

#undef L_TEMPLATE
#undef L_SELF
#undef L_TEMPLATE_SCOPE

} }

#endif
