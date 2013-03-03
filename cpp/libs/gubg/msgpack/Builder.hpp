#ifndef aoeuaoue
#define aoeuaoue

#include "gubg/msgpack/Write.hpp"

namespace gubg
{
    namespace msgpack
    {
        template <typename Buffer>
            class Builder
            {
                public:
                    Builder(Buffer &buffer):
                        buffer_(buffer), ok_(true), nrMA_(0) { }

                    bool valid() const { return ok_ && (nrMA_ == 0); }

                    class Array
                    {
                        public:
                            Array(Builder &builder, size_t nr):
                                outer_(builder), nr_(nr), ix_(0)
                        {
                            ++outer_.nrMA_;
                            write(buffer_(), nr_, ArrayTL_tag());
                        }
                            ~Array()
                            {
                                --outer_.nrMA_;
                                if (ix_ != nr_)
                                    outer_.ok_ = false;
                            }

                            Array &add(long l){return add_(l);}
                            Array &add(int l){return add_(l);}
                            Array &add_(long l)
                            {
                                if (ix_ >= nr_)
                                    outer_.ok_ = false;
                                else
                                {
                                    write(buffer_(), l);
                                    ++ix_;
                                }
                                return *this;
                            }
                            template <typename T>
                                Array &add(const T &t)
                                {
                                    if (ix_ >= nr_)
                                        outer_.ok_ = false;
                                    else
                                    {
                                        outer_.createMap(1).add(t);
                                        ++ix_;
                                    }
                                    return *this;
                                }

                            Buffer &buffer_(){return outer_.buffer_;}
                            Builder &outer_;
                            const size_t nr_;
                            size_t ix_;
                    };

                    class Map
                    {
                        public:
                            Map(Builder &builder, size_t nr):
                                outer_(builder), nr_(nr), ix_(0)
                        {
                            ++outer_.nrMA_;
                            write(buffer_(), nr_, MapTL_tag());
                        }
                            ~Map()
                            {
                                --outer_.nrMA_;
                                if (ix_ != nr_)
                                    outer_.ok_ = false;
                            }

                            Map &add(long id, long l)
                            {
                                if (ix_ >= nr_)
                                    outer_.ok_ = false;
                                else
                                {
                                    write(buffer_(), id);
                                    write(buffer_(), l);
                                    ++ix_;
                                }
                                return *this;
                            }
                            template <typename T>
                                Map &add(const T &t)
                                {
                                    if (ix_ >= nr_)
                                        outer_.ok_ = false;
                                    else
                                    {
                                        write(buffer_(), T::Id);
                                        t.serialize(outer_);
                                        ++ix_;
                                    }
                                    return *this;
                                }

                            Buffer &buffer_(){return outer_.buffer_;}
                            Builder &outer_;
                            const size_t nr_;
                            size_t ix_;
                    };

                    Array createArray(size_t nr)
                    {
                        return Array(*this, nr);
                    }
                    Map createMap(size_t nr)
                    {
                        return Map(*this, nr);
                    }

                private:
                    Buffer &buffer_;
                    bool ok_;
                    size_t nrMA_;
            };
    }
}

#endif
