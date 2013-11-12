#ifndef HEADER_garf_LED_hpp_ALREADY_INCLUDED
#define HEADER_garf_LED_hpp_ALREADY_INCLUDED

#include "Arduino.h"

namespace garf
{
    //Abuses the pullup resistor in input mode to act as a current limiting resistor. It is too large (20kO), but
    //some light manages to escape the LED on my mega
    class LED
    {
        public:
            void attach(uint8_t vcc, uint8_t gnd)
            {
                initialize_(vcc, gnd);
            }
            void on()
            {
                if (initialize_(vcc_, gnd_))
                    digitalWrite(vcc_, true);
            }
            void off()
            {
                if (initialize_(vcc_, gnd_))
                    digitalWrite(vcc_, false);
            }
        private:
            //Returns true if correctly initialized (now or in some call before this)
            bool initialize_(uint8_t vcc, uint8_t gnd)
            {
                if (vcc == InvalidPin || gnd == InvalidPin || vcc == gnd)
                    //Cannot initialize the pins with this data
                    return false;
                if (vcc == vcc_ && gnd == gnd_)
                    //Already initialized
                    return true;
                vcc_ = vcc;
                gnd_ = gnd;
                pinMode(vcc_, INPUT);  digitalWrite(vcc_, false);
                pinMode(gnd_, OUTPUT); digitalWrite(gnd_, false);
                return true;
            }
            static const uint8_t InvalidPin = 255;
            uint8_t vcc_ = InvalidPin;
            uint8_t gnd_ = InvalidPin;
    };
}

#endif
