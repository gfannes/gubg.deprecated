#include "gubg/chunky.hpp"
using namespace gubg;
using namespace std;

//Chunky::iterator
bool Chunky::iterator::operator==(const iterator &rhs) const
{
    return outer_ == rhs.outer_ && ix_ == rhs.ix_ && it_ == rhs.it_;
}
Chunky::iterator &Chunky::iterator::operator++()
{
    if (ix_ < outer_->chunks_.size())
    {
        if (it_ != outer_->chunks_[ix_].end())
            ++it_;
        else
        {
            if (++ix_ < outer_->chunks_.size())
                it_ = outer_->chunks_[ix_].begin();
            else
                it_ = outer_->chunk_.begin();
        }
    }
    else
        ++it_;
    return *this;
}
char Chunky::iterator::operator*() const
{
    return *it_;
}

//Chunky
Chunky::Chunky(size_t size):
    size_(size)
{
    chunk_.reserve(size_);
}

void Chunky::add(const string &str)
{
    if (chunk_.empty())
    {
        chunks_.push_back(str);
    }
    else if (chunk_.size()+str.size() < size_)
    {
        chunk_.append(str);
    }
    else
    {
        chunks_.push_back(chunk_);
        chunk_.assign(str);
    }
}
void Chunky::add(char ch)
{
    chunk_.push_back(ch);
    if (chunk_.size() >= size_)
    {
        chunks_.push_back(chunk_);
        chunk_.resize(0);
    }
}

size_t Chunky::size() const
{
    size_t res = chunk_.size();
    for (auto it = chunks_.begin(); it != chunks_.end(); ++it)
        res += it->size();
    return res;
}
bool Chunky::empty() const
{
    return chunk_.empty() && chunks_.empty();
}

void Chunky::clear()
{
    chunks_.clear();
    chunk_.clear();
}

Chunky::iterator Chunky::begin()
{
    iterator it;
    it.outer_ = this;
    if (chunks_.empty())
    {
        it.ix_ = 0;
        it.it_ = chunks_.front().begin();
    }
    else
    {
        it.ix_ = 1;
        it.it_ = chunk_.begin();
    }
    return it;
}
Chunky::iterator Chunky::end()
{
    iterator it;
    it.outer_ = this;
    it.ix_ = chunks_.size();
    it.it_ = chunk_.end();
    return it;
}

ostream &operator<<(ostream &os, const Chunky &chunky)
{
    for (auto it = chunky.chunks_.begin(); it != chunky.chunks_.end(); ++it)
        os << *it;
    os << chunky.chunk_;
    return os;
}
