#ifndef HEADER_gubg_Packer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Packer_hpp_ALREADY_INCLUDED

#define GUBG_MODULE_ "packer"
#include "gubg/log/begin.hpp"
namespace gubg { 

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

        struct SDU_out_tag {};
        struct PDU_out_tag {};
    } 

    template <typename Header, typename Body, typename Protocol>
        class Packer: public Protocol
        {
            private:
                typedef Packer<Header, Body, Protocol> Self;

            public:
                typedef packer::Reference<Self, Body, packer::SDU_out_tag> BodyRef_out;

                BodyRef_out sdu_out();

                void ref_dtor(packer::SDU_out_tag);

            private:
                Header header_;
                Body body_;
        };

    //Implementation
    template <typename Header, typename Body, typename Protocol>
        auto Packer<Header, Body, Protocol>::sdu_out() -> BodyRef_out
        {
            BodyRef_out br(*this, body_);
            return br;
        }
    template <typename Header, typename Body, typename Protocol>
        void Packer<Header, Body, Protocol>::ref_dtor(packer::SDU_out_tag)
        {
            Protocol::pack(header_, body_);
        }
} 
#include "gubg/log/end.hpp"

#endif
