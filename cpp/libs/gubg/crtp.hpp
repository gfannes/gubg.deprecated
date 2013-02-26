#ifndef HEADER_gubg_crtp_hpp_ALREADY_INCLUDED
#define HEADER_gubg_crtp_hpp_ALREADY_INCLUDED

namespace gubg
{
    template <typename Receiver>
        class CRTP
        {
            public:
#define GUBG_RECEIVER() Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
        };
}

#endif
