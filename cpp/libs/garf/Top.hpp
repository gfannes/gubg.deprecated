#ifndef HEADER_garf_Top_hpp_ALREADY_INCLUDED
#define HEADER_garf_Top_hpp_ALREADY_INCLUDED

#include "garf/Metronome.hpp"
#include "garf/Types.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/OnlyOnce.hpp"

namespace garf
{
    namespace top
    {
        template <typename Receiver, unsigned long Period>
            class Top_crtp: public Metronome_crtp<Top_crtp<Receiver, Period>, Period>
        {
            private:
                typedef Metronome_crtp<Top_crtp<Receiver, Period>, Period> Metronome;

            public:
                template <typename Elapse>
                void process(Elapse elapse)
                {
                    if (clearInfo_())
                        //We don't process this elapse data, it is probably too large due to sending our data
                        info_ = TopInfo();
                    else
                    {
                        if (elapse > info_.maxElapse)
                            info_.maxElapse = elapse;
                        ++info_.nrLoops;
                    }
                    Metronome::process(elapse);
                }
                void metronome_tick()
                {
                    serializer_.clear();
                    serializer_.serialize(info_);
                    clearInfo_.reset();
                    receiver_().top_msgpack(serializer_.buffer());
                }
            private:
                Receiver &receiver_() {return static_cast<Receiver&>(*this);}
                gubg::OnlyOnce clearInfo_;
                TopInfo info_;
                gubg::msgpack::Serializer<gubg::FixedVector<uint8_t, 20>, garf::TypeIds, 2> serializer_;
        };
    }
}

#endif
