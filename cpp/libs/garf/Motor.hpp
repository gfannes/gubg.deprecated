#ifndef HEADER_garf_Motor_hpp_ALREADY_INCLUDED
#define HEADER_garf_Motor_hpp_ALREADY_INCLUDED

#include "Arduino.h"

namespace garf
{
    template <int ForwardPin, int BackwardPin>
        class Motor
        {
            public:
                void setSpeed(int speed)
                {
                    if (speed >= 0)
                    {
                        if (speed > 255)
                            speed = 255;
                        forward_ = speed;
                        backward_ = 0;
                    }
                    else
                    {
                        speed = -speed;
                        if (speed > 255)
                            speed = 255;
                        backward_ = speed;
                        forward_ = 0;
                    }
                }

                template <typename Elapse>
                    void process(Elapse elapse)
                    {
                        initialize_();
                        analogWrite(ForwardPin, forward_);
                        analogWrite(BackwardPin, backward_);
                    }

            private:
                void initialize_()
                {
                    if (initialized_)
                        return;
                    pinMode(ForwardPin, OUTPUT);
                    pinMode(BackwardPin, OUTPUT);
                }
                bool initialized_ = false;
                unsigned char forward_ = 0;
                unsigned char backward_ = 0;
        };
}

#endif
