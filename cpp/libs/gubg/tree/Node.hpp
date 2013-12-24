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
				typedef std::vector<Ptr> Childs;

				Data data;
				Childs childs;

				static Ptr create(){return Ptr(new Node);}

				size_t nrChilds() const;

				void addChild(Ptr);

			private:
				Node(){}
		};

	//Implementation
	template <typename Data>
		size_t Node<Data>::nrChilds() const
		{
			return childs.size();
		}
	template <typename Data>
		void Node<Data>::addChild(Ptr child)
		{
			if (!child)
				return;
			childs.push_back(child);
		}

} }

#endif
