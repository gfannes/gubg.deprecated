#ifndef HEADER_gubg_IOBuffer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_IOBuffer_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg
{
    template <typename T>
        class IOBuffer
        {
            public:
                IOBuffer(size_t s):
                    buffer_(new T[s], s),
                    data_(buffer_.begin, 0){}
                IOBuffer(const std::string &str):
                    buffer_(new T[str.size()], str.size()),
                    data_(buffer_.begin, str.size())
            {
                str.copy(data_.begin, str.size());
            }
                ~IOBuffer()
                {
                    delete[] buffer_.begin;
                }

                void assign(const std::string &str)
                {
                    const size_t s = str.size();
                    if (buffer_.size() != s)
                    {
                        delete[] buffer_.begin;
                        buffer_.begin = new T[s];
                        buffer_.end = buffer_.begin+s;
                        data_.begin = buffer_.begin;
                        data_.end = buffer_.end;
                    }
                    str.copy(data_.begin, s);
                }

                void clear()
                {
                    data_.begin = data_.end = buffer_.begin;
                }

                size_t size() const {return data_.size();}
                size_t freeSize() const {return buffer_.end - data_.end;}
                bool empty() const {return data_.size() == 0;}
                bool full() const {return freeSize() == 0;}

                std::string str() const {return std::string(data(), size());}

                const T *data() const {return data_.begin;}
                T *data() {return data_.begin;}
                T *freeData() {return data_.end;}

                bool push_back(T t)
                {
                    if (full())
                        return false;
                    *data_.end++ = t;
                    return true;
                }

                template <typename Data>
                bool write(const Data &data)
                {
                    if (freeSize() < data.size())
                        return false;
                    for (auto t: data)
                        push_back(t);
                    return true;
                }

                bool scrollBegin(size_t nr)
                {
                    if (nr > size())
                        return false;
                    data_.begin += nr;
                    return true;
                }
                bool scrollEnd(size_t nr)
                {
                    if (nr > freeSize())
                        return false;
                    data_.end += nr;
                    return true;
                }

            private:
                struct Range
                {
                    T *begin;
                    T *end;
                    Range(T *buf, size_t size):
                        begin(buf),
                        end(begin+size){}
                    size_t size() const {return end-begin;}
                };
                Range buffer_;
                Range data_;
        };
}

#endif
