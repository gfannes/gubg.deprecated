#ifndef HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED

#include "gubg/cnt_ptr.hpp"
#include <memory>
#include <cassert>

#define GUBG_MODULE "N"
#include "gubg/log/begin.hpp"
namespace gubg { namespace tree { 

	namespace impl { 
		template <typename Data>
		struct Node
		{
#if 1
			typedef std::shared_ptr<Node> Ptr;
			typedef std::weak_ptr<Node> WPtr;
#else
			typedef gubg::cnt_ptr<Node> Ptr;
			typedef gubg::cnt_ptr<Node> WPtr;
#endif
			//Childs
			Ptr first;
			Ptr last;
			//Siblings
			Ptr prev;
			Ptr next;
			//Parent
			WPtr parent;
			//Data
			Data data;

			bool has_child() const {assert(invariants_()); return (bool)first;}

			bool invariants_() const
			{
				assert((first and last) or (!first and !last));
				return true;
			}
		};
	} 

	template <typename Data>
		class Node
		{
			public:
				static Node create()
				{
					Node n;
					n.impl_.reset(new Node_i);
					return n;
				}

				operator bool () const {return (bool)impl_;}

				//Make sure the node is valid (using operator bool) before trying to call any of the following methods

				size_t nrChilds() const
				{
					assert(impl_);
					assert(impl_->invariants_());
					size_t nr = 0;
					auto ch = impl_->first;
					while (ch)
					{
						++nr;
						ch = ch->next;
					}
					return nr;
				}

				Node firstChild() const
				{
					S();
					assert(impl_);
					Node_i &si = *impl_;
					assert(si.invariants_());
					Node n;
					n.impl_ = si.first;
					return n;
				}
				Node lastChild() const
				{
					S();
					assert(impl_);
					Node_i &si = *impl_;
					assert(si.invariants_());
					Node n;
					n.impl_ = si.last;
					return n;
				}
				Node nextSibling() const
				{
					S();
					assert(impl_);
					Node_i &si = *impl_;
					assert(si.invariants_());
					Node n;
					n.impl_ = si.next;
					return n;
				}
				Node prevSibling() const
				{
					S();
					assert(impl_);
					Node_i &si = *impl_;
					assert(si.invariants_());
					Node n;
					n.impl_ = si.prev;
					return n;
				}

				void pushChild(Node ch)
				{
					S();
					assert(impl_);
					Node_i &si = *impl_;
					assert(si.invariants_());

					assert(ch.impl_);
					Node_i &chi = *ch.impl_;
					assert(chi.invariants_());

					chi.parent = impl_;
					chi.next.reset();
					if (si.has_child())
					{
						L("We already have at least one child");
						chi.prev = si.last;
						si.last->next = ch.impl_;
						si.last = ch.impl_;
					}
					else
					{
						L("This is the first child of self");
						chi.prev.reset();
						si.first = ch.impl_;
						si.last = ch.impl_;
					}

					assert(si.invariants_());
				}
				Node shiftChild()
				{
					S();
					assert(impl_);
					Node_i &si = *impl_;
					assert(si.invariants_());

					Node n;

					if (si.has_child())
					{
						L("We have at least one child");
						n.impl_ = si.first;
						{
							auto &ni = *n.impl_;
							ni.parent.reset();
							//This is the first child, it should not have a prev sibling
							assert(not ni.prev);
							//There might be a next though, which should not be lost
							si.first = ni.next;
							ni.next.reset();
						}
						//Make sure we adhere to the invariants
						if (si.first)
							si.first->prev.reset();
						else
							si.last.reset();
					}

					assert(si.invariants_());
					assert((not n.impl_) or n.impl_->invariants_());
					return n;
				}

				Data &      data()       {assert(impl_); return impl_->data;}
				const Data &data() const {assert(impl_); return impl_->data;}

			private:
				typedef impl::Node<Data> Node_i;
				typename Node_i::Ptr impl_;
		};
} } 
#include "gubg/log/end.hpp"

#endif
