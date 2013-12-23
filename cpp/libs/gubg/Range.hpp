#ifndef HEADER_gubg_Range_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Range_hpp_ALREADY_INCLUDED

#include <iterator>
#include <cassert>

namespace gubg
{
    template <typename Iterator_>
        class Range
        {
            public:
                typedef Iterator_ Iterator;

                Range();
                Range(Iterator begin, Iterator end);
                template <typename Container>
                    Range(Container &);

                //Basic range functionality that requires the minimum functionality from Iterator:
                //* operator==()
                //* operator*()
                //* operator++()
                bool empty() const;
                typename std::iterator_traits<Iterator>::value_type &front() const;
                void popFront();

                //Requires operator<()
                bool contains(Iterator) const;

				Iterator begin() const;
				Iterator end() const;

				void begin(Iterator);
				void end(Iterator);
				
				void clear();

            private:
                Iterator begin_;
                Iterator end_;
        };
    template <typename Container>
        Range<typename Container::iterator> make_range(Container &);

    //Implementation
    template <typename Iterator_>
        Range<Iterator_>::Range():
            begin_(), end_()
    {
        assert(empty());
    }
    template <typename Iterator_>
        Range<Iterator_>::Range(Iterator begin, Iterator end):
            begin_(begin), end_(end) {}
    template <typename Iterator_>
        template <typename Container>
        Range<Iterator_>::Range(Container &container):
            begin_(std::begin(container)), end_(std::end(container)) {}
    template <typename Iterator_>
        bool Range<Iterator_>::empty() const
        {
            return begin_ == end_;
        }
    template <typename Iterator_>
        typename std::iterator_traits<Iterator_>::value_type &Range<Iterator_>::front() const
        {
            return *begin_;
        }
    template <typename Iterator_>
        void Range<Iterator_>::popFront()
        {
            assert(!empty());
            ++begin_;
        }
    template <typename Iterator_>
        bool Range<Iterator_>::contains(Iterator it) const
        {
            if (it < begin_)
                return false;
            return it < end_;
        }
    template <typename Iterator_>
		Iterator_ Range<Iterator_>::begin() const
		{
			return begin_;
		}
    template <typename Iterator_>
		Iterator_ Range<Iterator_>::end() const
		{
			return end_;
		}
    template <typename Iterator_>
		void Range<Iterator_>::begin(Iterator b)
		{
			begin_ = b;
		}
    template <typename Iterator_>
		void Range<Iterator_>::end(Iterator e)
		{
			end_ = e;
		}
    template <typename Iterator_>
		void Range<Iterator_>::clear()
		{
			*this = Range();
		}
	//Free functions
	template <typename Container>
		Range<typename Container::iterator> make_range(Container &container)
		{
			return Range<typename Container::iterator>(container);
		}
}

#endif
