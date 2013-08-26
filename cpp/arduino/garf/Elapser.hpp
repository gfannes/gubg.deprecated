#ifndef HEADER_garf_Elapser_hpp_ALREADY_INCLUDED
#define HEADER_garf_Elapser_hpp_ALREADY_INCLUDED

#include "Arduino.h"

namespace garf
{
    class Elapser
    {
        public:
            Elapser(): initialized_(false) {}

            void process()
            {
                if (!initialized_)
                {
                    previousProcess_ = millis();
                    elapse_ = 0;
                    initialized_ = true;
                }

                const unsigned long now = millis();
                elapse_ = now - previousProcess_;
                previousProcess_ = now;
            }

            unsigned int elapse() const {return elapse_;}

        private:
            unsigned long previousProcess_;
            unsigned int elapse_;
            bool initialized_;
    };
}

#endif
