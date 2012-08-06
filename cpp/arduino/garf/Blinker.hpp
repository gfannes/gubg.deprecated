#ifndef garf_Blinker_hpp
#define garf_Blinker_hpp

namespace garf
{
    template <long Timeout>
    class Blinker
    {
        public:
            Blinker():
                state_(DoBlink),
                blinkTimeout_(Timeout)
            {
                pinMode(LED, OUTPUT);
            }
            void boot()
            {
                for (int i = 0; i < 30; ++i)
                {
                    digitalWrite(LED, HIGH);
                    delay(50);
                    digitalWrite(LED, LOW);
                    delay(50);
                }
            }
            void process(int elapse)
            {
                if (state_ & DoBlink)
                {
                    while (elapse > blinkTimeout_)
                    {
                        elapse -= blinkTimeout_;
                        blinkTimeout_ = Timeout;
                        state_ ^= IsOn;
                    }
                    blinkTimeout_ -= elapse;
                }
                digitalWrite(LED, ((state_ & IsOn) ? HIGH : LOW));
            }
            void set(bool blink, bool on)
            {
                if (blink)
                    state_ |= DoBlink;
                else
                    state_ &= ~DoBlink;
                if (on)
                    state_ |= IsOn;
                else
                    state_ &= ~IsOn;
                blinkTimeout_ = Timeout;
            }
        private:
            static const int LED = 13;

            long blinkTimeout_;

            unsigned char state_;
            static const unsigned char IsOn = 0x01;
            static const unsigned char DoBlink = 0x02;
    };
}

#endif
