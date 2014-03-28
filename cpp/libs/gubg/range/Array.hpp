#ifndef aoeuaoeuArray
#define aoeuaoeuArray

#include "gubg/range/RawArray.hpp"

namespace gubg { namespace range { 

	template <typename T>
		class Array
		{
			public:
				Array(T *begin, T *end): all_(begin, end) {}

				bool empty() const {return sub_.empty();}

				const T &front() const {return sub_.front();}
				T &front() {return sub_.front();}
				const T &back() const {return sub_.back();}
				T &back() {return sub_.back();}

				void popFront() {sub_.popFront();}
				void popBack()  {sub_.popBack();}

				//Range remains unchanged upon failure
				bool shrinkFront(size_t nr) {return sub_.shrinkFront(nr);}
				bool shrinkBack(size_t nr)  {return sub_.shrinkBack(nr);}

				bool growFront(size_t nr)
				{
					if (sub_.begin()-all_.begin() < nr)
						return false;
					sub_.growFront(nr);
					return true;
				}
				bool growBack(size_t nr)
				{
					if (all_.end()-sub_.end() < nr)
						return false;
					sub_.growBack(nr);
					return true;
				}

			private:
				raw::Array<T> all_;
				raw::Array<T> sub_ = all_;
		};

} } 

#endif
