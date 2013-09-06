#ifndef HEADER_garf_IRSensor_hpp_ALREADY_INCLUDED
#define HEADER_garf_IRSensor_hpp_ALREADY_INCLUDED

namespace garf
{
    template <typename Receiver, int Pin>
        class IRSensor
        {
            public:
                void init()
                {
                    pinMode(Pin, INPUT);
                    state_ = -1;
                }
                void process()
                {
                    const auto s = digitalRead(Pin);
                    if (state_ != s)
                        receiver_().irsensor_detect(!(state_ = s));
                }
            private:
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                int state_;
        };
}

#endif
