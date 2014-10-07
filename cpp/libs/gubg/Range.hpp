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
				typedef Range<Iterator_> Self;
                typedef Iterator_ Iterator;

                Range();
                Range(Iterator begin, Iterator end);
                template <typename Container>
                    Range(Container &);
                template <typename Container>
                    Range(const Container &);
				Self &operator=(const Self &);
				template <typename OtherIterator>
					Self &operator=(const Range<OtherIterator> &);

                //Basic range functionality that requires the minimum functionality from Iterator:
                //* operator==()
                //* operator*()
                //* operator++()
                bool empty() const;
                typename std::iterator_traits<Iterator>::reference front();
                const typename std::iterator_traits<Iterator>::reference front() const;
                void popFront();

                //Requires operator<()
                bool contains(Iterator) const;
                bool contains(std::function<bool(const typename Iterator::value_type &)>) const;

				Iterator begin() const;
				Iterator end() const;

				void begin(Iterator);
				void end(Iterator);
				
				void clear();

            private:
				template <typename OtherIterator>
					friend class Range;

                Iterator begin_;
                Iterator end_;
        };
    template <typename Container>
        Range<typename Container::iterator> make_range(Container &);
    template <typename Container>
        Range<typename Container::const_iterator> make_range(const Container &);

    //Implementation
#define L_TEMPLATE template <typename Iterator_>
#define L_TYPE Range<Iterator_>
	L_TEMPLATE
        L_TYPE::Range():
            begin_(), end_()
    {
        assert(empty());
    }
	L_TEMPLATE
        L_TYPE::Range(Iterator begin, Iterator end):
            begin_(begin), end_(end) {}
	L_TEMPLATE
        template <typename Container>
        L_TYPE::Range(Container &container):
            begin_(std::begin(container)), end_(std::end(container)) {}
	L_TEMPLATE
        template <typename Container>
        L_TYPE::Range(const Container &container):
            begin_(std::begin(container)), end_(std::end(container)) {}
	L_TEMPLATE
		typename L_TYPE::Self &L_TYPE::operator=(const Self &rhs)
		{
			if (this != &rhs)
			{
				begin_ = rhs.begin_;
				end_ = rhs.end_;
			}
			return *this;
		}
	L_TEMPLATE
		template <typename OtherIterator>
		typename L_TYPE::Self &L_TYPE::operator=(const Range<OtherIterator> &rhs)
		{
			begin_ = rhs.begin_;
			end_ = rhs.end_;
			return *this;
		}
	L_TEMPLATE
        bool L_TYPE::empty() const
        {
            return begin_ == end_;
        }
	L_TEMPLATE
        typename std::iterator_traits<Iterator_>::reference L_TYPE::front()
        {
            return *begin_;
        }
	L_TEMPLATE
        const typename std::iterator_traits<Iterator_>::reference L_TYPE::front() const
        {
            return *begin_;
        }
	L_TEMPLATE
        void L_TYPE::popFront()
        {
            assert(!empty());
            ++begin_;
        }
	L_TEMPLATE
        bool L_TYPE::contains(Iterator it) const
        {
            if (it < begin_)
                return false;
            return it < end_;
        }
	L_TEMPLATE
        bool L_TYPE::contains(std::function<bool(const typename Iterator::value_type &)> ftor) const
        {
            for (auto it = begin_; it != end_; ++it)
                if (ftor(*it))
                    return true;
            return false;
        }
	L_TEMPLATE
        Iterator_ L_TYPE::begin() const
        {
            return begin_;
        }
	L_TEMPLATE
        Iterator_ L_TYPE::end() const
        {
            return end_;
        }
	L_TEMPLATE
        void L_TYPE::begin(Iterator b)
        {
            begin_ = b;
        }
	L_TEMPLATE
        void L_TYPE::end(Iterator e)
        {
            end_ = e;
        }
	L_TEMPLATE
        void L_TYPE::clear()
        {
            *this = Range();
        }
#undef L_TYPE
#undef L_TEMPLATE
    //Free functions
    template <typename Container>
        Range<typename Container::iterator> make_range(Container &container)
        {
            return Range<typename Container::iterator>(container);
        }
    template <typename Container>
        Range<typename Container::const_iterator> make_range(const Container &container)
        {
            return Range<typename Container::const_iterator>(container);
        }
}

#endif
