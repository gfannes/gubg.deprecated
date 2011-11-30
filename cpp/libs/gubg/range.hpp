#ifndef gubg_range_hpp
#define gubg_range_hpp

namespace gubg
{
    template <typename Container, typename Wanted>
        class Range
        {
            public:
                Range(Container &container):
                    here_(container.begin()), end_(container.end())
                {
                    scroll_();
                }
                bool empty() const {return end_ == here_;}
                Wanted front() const {return dynamic_cast<Wanted>(*here_);}
                void popFront()
                {
                    if (!empty())
                        ++here_;
                    scroll_();
                }
                Wanted operator->() const
                {
                    return front();
                }
            private:
                void scroll_()
                {
                    while (!empty())
                    {
                        if (dynamic_cast<Wanted>(*here_))
                            break;
                        ++here_;
                    }
                }

                typedef typename Container::iterator Iterator;
                Iterator here_;
                const Iterator end_;
        };

    template <typename Wanted, typename Container>
        Range<Container, Wanted> rangeOnly(Container &container)
        {
            return Range<Container, Wanted>(container);
        }
}

#endif
