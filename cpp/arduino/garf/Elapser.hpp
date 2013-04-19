#ifndef HEADER_garf_Elapser_hpp_ALREADY_INCLUDED
#define HEADER_garf_Elapser_hpp_ALREADY_INCLUDED

#include "Arduino.h"

namespace garf
{
    class Elapser
    {
        public:
            Elapser():
                previousProcess_(millis()),
                elapse_(0){}

            void process()
            {
                const long now = millis();
                elapse_ = now - previousProcess_;
                previousProcess_ = now;
            }
            int elapse() const {return elapse_;}

        private:
            long previousProcess_;
            int elapse_;
    };
}

#endif
