#ifndef HEADER_garf_Sweeper_hpp_ALREADY_INCLUDED
#define HEADER_garf_Sweeper_hpp_ALREADY_INCLUDED

#include "garf/Global.hpp"
#include "Arduino.h"
#include "Servo/Servo.h"

namespace garf
{
    template <int Pin, int Min, int Max, int Delay>
        class Sweeper
        {
            private:
                typedef Sweeper<Pin, Min, Max, Delay> Self;
                //This can only be created from Global to make sure no calls to
                //Servo are made before the lib was initialized
                template <typename T, int IX>
                    friend class Global;

                Sweeper():
                    pos_(Min),
                    d_(1)
                {
                    servo_.attach(Pin);
                }

            public:
                void process()
                {
                    pos_ += d_;
                    servo_.write(pos_);
                    if (pos_ <= Min || pos_ >= Max)
                        d_ = -d_;
                    delay(Delay);
                }

            private:
                Servo servo_;
                int pos_;
                int d_;
        };
}

#endif
