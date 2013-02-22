#ifndef aoeuaoeuaoeu
#define aoeuaoeuaoeu

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
