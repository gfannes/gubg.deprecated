#ifndef HEADER_garf_Motor_hpp_ALREADY_INCLUDED
#define HEADER_garf_Motor_hpp_ALREADY_INCLUDED

namespace garf
{
    template <int ForwardPin, int BackwardPin>
        class Motor
        {
            public:
                Motor()
                {
                    setSpeed(0);
                }

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
                    analogWrite(ForwardPin, forward_);
                    analogWrite(BackwardPin, backward_);
                }

            private:
                unsigned char forward_;
                unsigned char backward_;
        };
}

#endif
