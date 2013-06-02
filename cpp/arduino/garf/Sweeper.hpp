#ifndef HEADER_garf_Sweeper_hpp_ALREADY_INCLUDED
#define HEADER_garf_Sweeper_hpp_ALREADY_INCLUDED

#include "Arduino.h"
#include "Servo/Servo.h"

namespace garf
{
    template <int Pin, int Min, int Max>
        class Sweeper
        {
            public:
                Sweeper():
                    pos_(Min),
                    d_(1)
                {
                    servo_.attach(Pin);
                }
                void init()
                {
                    pos_ = Min;
                    d_ = 1;
                    servo_.attach(Pin);
                }
                void process()
                {
                    pos_ += d_;
                    servo_.write(pos_);
                    if (pos_ <= Min || pos_ >= Max)
                        d_ = -d_;
                    delay(155);
                }
            private:
                Servo servo_;
                int pos_;
                int d_;
        };
}

#endif
