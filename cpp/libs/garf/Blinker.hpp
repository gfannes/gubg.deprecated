#ifndef HEADER_garf_Blinker_hpp_ALREADY_INCLUDED
#define HEADER_garf_Blinker_hpp_ALREADY_INCLUDED

#include "Arduino.h"
#include "garf/Metronome.hpp"

namespace garf
{
    template <long Period, int LED_ = 13>
    class Blinker: public Metronome_crtp<Blinker<Period, LED_>, Period>
    {
        public:
            typedef uint8_t Pattern;

            static const int LED = LED_;

            void setPattern(Pattern pattern)
            {
                doInitialization_();

                pattern_ = pattern;

                setLED_();
            }

            void metronome_tick()
            {
                doInitialization_();

                setLED_();

                ++ix_;
            }

        private:
            void doInitialization_()
            {
                if (ix_ != InvalidIX_)
                    return;

                //We delay initialization beyond the ctor to make sure
                //normal arduino setup was done
                pinMode(LED, OUTPUT);
                ix_ = 0;

                setLED_();
            }
            void setLED_()
            {
                if (ix_ >= sizeof(Pattern)*8)
                    ix_ = 0;
                digitalWrite(LED, ((pattern_ & (1 << ix_)) ? HIGH : LOW));
            }

            //If ix_ == InvalidIX, initialization is still needed
            static const unsigned char InvalidIX_ = 0xff;
            unsigned char ix_ = InvalidIX_;
            Pattern pattern_ = 0;
    };
}

#endif
