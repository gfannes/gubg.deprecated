#ifndef HEADER_garf_Elapser_hpp_ALREADY_INCLUDED
#define HEADER_garf_Elapser_hpp_ALREADY_INCLUDED

#include "Arduino.h"
#include "gubg/OnlyOnce.hpp"

namespace garf
{
    class Elapser
    {
        public:
            void process()
            {
                if (doInit_())
                {
                    previousProcess_ = millis();
                    elapse_ = 0;
                }

                const unsigned long now = millis();
                elapse_ = now - previousProcess_;
                previousProcess_ = now;
            }

            unsigned int elapse() const {return elapse_;}

        private:
            unsigned long previousProcess_;
            unsigned int elapse_;
            gubg::OnlyOnce doInit_;
    };
}

#endif
