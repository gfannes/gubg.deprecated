#ifndef HEADER_gubg_Packer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Packer_hpp_ALREADY_INCLUDED

#include <cassert>

#define GUBG_MODULE_ "packer"
#include "gubg/log/begin.hpp"
namespace gubg { 

    //Forward declaration
    template <typename Header, typename Body, typename Protocol>
        class Packer;

    namespace packer { 
        template <typename Outer, typename T, typename Tag>
            class Reference
            {
                public:
                    T &ref() {return ref_;}
                    const T &ref() const {return ref_;}

                    Reference(Outer &outer, T &ref): outer_(outer), ref_(ref) {}
                    ~Reference()
                    {
                        outer_.ref_dtor(Tag());
                    }

                private:
                    Outer &outer_;
                    T &ref_;
            };

        template <typename Value>
            class Range_memory
            {
                public:
                    typedef Value value_type;

                    Range_memory(const Value *buffer, size_t size): front_(buffer), end_(buffer + size) {}

                    bool empty() const {return front_ == end_;}
                    value_type front() const
                    {
                        assert(!empty());
                        assert(front_);
                        return *front_;
                    }
                    void pop_front()
                    {
                        assert(!empty());
                        assert(front_);
                        ++front_;
                    }

                private:
                    const Value *front_ = 0;
                    const Value *end_ = 0;
            };

        template <typename R1, typename R2>
            class RangePair
            {
                public:
                    typedef typename R1::value_type value_type;

                    RangePair(R1 r1, R2 r2): r1_(r1), r2_(r2) {}

                    bool empty() const {return r1_.empty() and r2_.empty();}

                    value_type front() const
                    {
                        if (!r1_.empty())
                            return r1_.front();
                        return r2_.front();
                    }

                    void pop_front()
                    {
                        if (!r1_.empty())
                        {
                            r1_.pop_front();
                            return;
                        }
                        r2_.pop_front();
                    }

                private:
                    R1 r1_;
                    R2 r2_;
            };

        //Traits to know the range corresponding to some buffer, and how to create it
        template <typename Buffer>
            struct Traits { };
        template <>
            struct Traits<std::string>
            {
                typedef Range_memory<char> Range;

                static Range create(const std::string &str)
                {
                    return Range(&str[0], str.size());
                }
            };
        template <typename Header, typename Body, typename Protocol>
            struct Traits<Packer<Header, Body, Protocol>>
            {
                typedef RangePair<typename Traits<Header>::Range, typename Traits<Body>::Range> Range;

                static Range create(const Packer<Header, Body, Protocol> &p)
                {
                    return create(p.header(), p.body());
                }
                static Range create(const Header &header, const Body &body)
                {
                    return Range(Traits<Header>::create(header), Traits<Body>::create(body));
                }
            };

        struct SDU_out_tag {};
        struct PDU_out_tag {};
    } 

    template <typename Header, typename Body, typename Protocol>
        class Packer: public Protocol
    {
        private:
            typedef Packer<Header, Body, Protocol> Self;

        public:
            typedef packer::Reference<Self, Body, packer::SDU_out_tag> SDURef_out;
            typedef packer::RangePair<typename packer::Traits<Header>::Range, typename packer::Traits<Body>::Range> PDURange;

            SDURef_out sdu_out();

            PDURange pdu_out();

            void ref_dtor(packer::SDU_out_tag);

            const Header &header() const {return header_;}
            const Body &body() const {return body_;}

        private:
            Header header_;
            Body body_;
    };

    //Implementation
    template <typename Header, typename Body, typename Protocol>
        auto Packer<Header, Body, Protocol>::sdu_out() -> SDURef_out
        {
            SDURef_out br(*this, body_);
            return br;
        }
    template <typename Header, typename Body, typename Protocol>
        void Packer<Header, Body, Protocol>::ref_dtor(packer::SDU_out_tag)
        {
            Protocol::pack(header_, body_);
        }
    template <typename Header, typename Body, typename Protocol>
        auto Packer<Header, Body, Protocol>::pdu_out() -> PDURange
        {
            return packer::Traits<Self>::create(*this);
        }
} 
#include "gubg/log/end.hpp"

#endif
