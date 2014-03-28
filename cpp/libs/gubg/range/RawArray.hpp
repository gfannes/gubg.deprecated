#ifndef HEADER_gubg_range_RawArray_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_RawArray_hpp_ALREADY_INCLUDED

#include <cstddef>

namespace gubg { namespace range { namespace raw { 

    template <typename Value>
        class Array
        {
			public:
				typedef Value value_type;

				Array(value_type *begin, value_type *end): begin_(begin), end_(end) {}

				size_t size() const {return end_ - begin_;}

				value_type *begin() const {return begin_;}
				value_type *end() const {return end_;}

				bool empty() const {return begin_ == end_;}

				value_type &front() {return *begin_;}
				const value_type &front() const {return *begin_;}

				value_type &back() {return *(end_-1);}
				const value_type &back() const {return *(end_-1);}

				void popFront() {++begin_;}
				void popBack() {--end_;}

				//Remains unchanged upon failure
				bool shrinkFront(size_t nr)
				{
					if (size() < nr)
						return false;
					begin_ += nr;
					return true;
				}
				bool shrinkBack(size_t nr)
				{
					if (size() < nr)
						return false;
					end_ -= nr;
					return true;
				}

				//Dangerous: unchecked operation
				//The caller should ensure that begin_ and end_ remain within valid bounds
				void growFront(size_t nr) {begin_ -= nr;}
				void growBack(size_t nr)  {end_ += nr;}

			private:
				Value *begin_;
				Value *end_;
        };

} } } 

#endif
