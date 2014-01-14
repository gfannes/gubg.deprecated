#ifndef HEADER_rinle_model_Node_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Node_hpp_ALREADY_INCLUDED

#include <memory>

namespace rinle { namespace model { namespace tree {

	template <typename NodePtr>
        void addChild(NodePtr parent, NodePtr child);

	template <typename Data>
		class Node
		{
			public:
				typedef std::shared_ptr<Node<Data>> Ptr;
				typedef std::weak_ptr<Node<Data>> WPtr;

				static Ptr create();
				static Ptr create(Ptr parent);

                Data &data() {return data_;}
                const Data &data() const {return data_;}

                Ptr parent() {return parent_.lock();}
                Ptr prev()   {return prev_;}
                Ptr next()   {return next_;}
                Ptr first()  {return first_;}
                Ptr last()   {return last_;}

			private:
                template <typename NodePtr>
                    friend void addChild(NodePtr parent, NodePtr child);

				WPtr parent_;
				Ptr prev_, next_;
				Ptr first_, last_;

				Data data_;
		};

	template <typename Data>
		typename Node<Data>::Ptr Node<Data>::create()
		{
			return Ptr(new Node);
		}
	template <typename Data>
		typename Node<Data>::Ptr Node<Data>::create(Ptr parent)
		{
            Node<Data>::Ptr ret = create();
            addChild(parent, ret);
            return ret;
		}

	template <typename NodePtr>
        void addChild(NodePtr parent, NodePtr child)
        {
            if (!parent || !child)
                return;

            child->parent_ = parent;
            auto &p = *parent;

            if (!p.last_)
            {
                //We are the first child of parent
                p.first_ = p.last_ = child;
                return;
            }

            //Parent already has some childs
            p.last_->next_ = child;
            child->prev_ = p.last_;
            p.last_ = child;
        }

} } }

#endif
