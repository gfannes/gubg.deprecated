#ifndef gubg_SmartRange_hpp
#define gubg_SmartRange_hpp

namespace gubg
{
    template <typename Container>
        class SmartRange
        {
            public:
                typedef typename Container::iterator iterator;
                typedef typename Container::value_type value_type;
                static const size_t npos = -1;

                SmartRange(): begin_(), end_(){}
                SmartRange(Container &&container):
                    data_(new Container(std::move(container))),
                    begin_(data_->begin()),
                    end_(data_->end()){}
                SmartRange(const SmartRange &rhs):
                    data_(rhs.data_),
                    begin_(rhs.begin_),
                    end_(rhs.end_){}
                SmartRange &operator=(const SmartRange &rhs)
                {
                    if (this != &rhs)
                    {
                        data_ = rhs.data_;
                        begin_ = rhs.begin_;
                        end_ = rhs.end_;
                    }
                    return *this;
                }
                SmartRange &operator=(Container &&container)
                {
                    data_.reset(new Container(std::move(container)));
                    begin_ = data_->begin();
                    end_ = data_->end();
                    return *this;
                }

                iterator begin() const {return begin_;}
                iterator end() const {return end_;}
                bool empty() const {return begin_ >= end_;}
                size_t size() const {return end_ - begin_;}
                value_type *data() const {return &*begin_;};

                void popFront() {++begin_;}
                value_type &front() {return *begin_;}
                value_type &back() {return *(end_-1);}

                value_type &operator[](const size_t ix){return *(begin_+ix);}

                SmartRange subrange(const size_t pos, const size_t count = npos)
                {
                    SmartRange sr(*this);
                    sr.begin_ += pos;
                    if (npos != count && count < sr.size())
                        sr.end_ = sr.begin_ + count;
                    return sr;
                }

                size_t find(const SmartRange &needle, size_t pos = 0) const
                {
                    return find_(needle, pos);
                }
                size_t find(const Container &needle, size_t pos = 0) const
                {
                    return find_(needle, pos);
                }

                template <typename Tokens, typename Splitter>
                void breakdown(Tokens &tokens, Splitter splitter)
                {
                    tokens.clear();
                    SmartRange range(*this);
                    if (range.empty())
                        return;
                    iterator start = range.begin();
                    while (true)
                    {
                        value_type &prev = range.front();
                        range.popFront();
                        if (range.empty())
                            break;
                        value_type &current = range.front();
                        if (splitter(prev, current))
                        {
                            tokens.push_back(SmartRange(data_, start, range.begin()));
                            start = range.begin();
                        }
                    }
                    tokens.push_back(SmartRange(data_, start, range.begin()));
                }

            private:
                typedef std::shared_ptr<Container> Data;
                SmartRange(Data data, iterator begin, iterator end):
                    data_(data),
                    begin_(begin),
                    end_(end){}

                template <typename Needle>
                size_t find_(const Needle &needle, size_t pos) const
                {
                    if (pos >= size())
                        return npos;
                    if (needle.empty())
                        return npos;
                    const size_t maxPos = size()-pos+1-needle.size();
                    const size_t needleSize = needle.size();
                    size_t i;
                    for (; pos <= maxPos; ++pos)
                    {
                        const value_type *h = &*(begin_+pos);
                        const value_type *n = needle.data();
                        for (i = 0; i < needleSize; ++i)
                            if (*n != *h)
                                break;
                        if (i == needleSize)
                            //We found a full match
                            return pos;
                    }
                    return npos;
                }

                Data data_;
                iterator begin_;
                iterator end_;
        };
    template <typename Container>
        const size_t SmartRange<Container>::npos;
}

#endif
