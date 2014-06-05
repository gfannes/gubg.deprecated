#ifndef HEADER_gubg_range_Range_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_Range_hpp_ALREADY_INCLUDED

namespace gubg { namespace range { 

	template <typename T, typename Data, typename Sub>
		class Range: public Data, public Sub
		{
			public:
				Range(): Sub(Data::ref()) {}

				typedef T value_type;

				bool empty() const {return Sub::empty();}

				value_type &front() {return Sub::front(Data::ref());}
				const value_type &front() const {return Sub::front(Data::ref());}

				value_type &back() {return Sub::back(Data::ref());}
				const value_type &back() const {return Sub::back(Data::ref());}
		};

} } 

#endif
