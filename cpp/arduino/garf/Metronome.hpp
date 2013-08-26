#ifndef HEADER_garf_Metronome_hpp_ALREADY_INCLUDED
#define HEADER_garf_Metronome_hpp_ALREADY_INCLUDED

#include "Arduino.h"

namespace garf
{
    template <typename Receiver, unsigned long Period>
        class Metronome_crtp
        {
            public:
                Metronome_crtp(): cumul_(0) {}

                template <typename Elapse>
                    void process(Elapse elapse)
                    {
                        cumul_ += elapse;
                        while (cumul_ > Period)
                        {
                            receiver_().metronome_tick();
                            cumul_ -= Period;
                        }
                    }

            private:
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                unsigned long cumul_;
        };
}

#endif
