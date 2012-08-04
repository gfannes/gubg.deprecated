#ifndef garf_Status_hpp
#define garf_Status_hpp

#include "Arduino.h"

namespace garf
{
    class Status
    {
        public:
            Status():
                state_(false),
                lastToggle_(millis())
            {
                pinMode(LED, OUTPUT);
            }
            void process()
            {
                const long now = millis();
                if (now - lastToggle_ > 500)
                {
                    state_ = !state_;
                    digitalWrite(LED, (state_ ? HIGH : LOW));
                    lastToggle_ = now;
                }
            }
        private:
            static const int LED = 13;
            long lastToggle_;
            bool state_;
    };
}

#endif
