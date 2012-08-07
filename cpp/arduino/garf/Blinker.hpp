#ifndef garf_Blinker_hpp
#define garf_Blinker_hpp

namespace garf
{
    enum class BlinkMode: unsigned char { Flat, Normal, Fast};

    template <long NormalTimeout_, int LED_ = 13>
    class Blinker
    {
        private:
            static const unsigned char StateMask = 0x80;
            static const unsigned char ModeMask = 0x7f;

        public:
            static const long NormalTimeout = NormalTimeout_;
            static const long FastTimeout = NormalTimeout_/2;
            static const long LED = LED_;

            Blinker():
                state_((unsigned char)BlinkMode::Flat & StateMask),
                timeout_(0)
            {
                pinMode(LED, OUTPUT);
                digitalWrite(LED, HIGH);
            }
            void set(BlinkMode mode)
            {
                const BlinkMode currentMode = BlinkMode(state_ & ModeMask);
                if (mode == currentMode)
                    return;
                switch (mode)
                {
                    case BlinkMode::Flat:   timeout_ = 0; break;
                    case BlinkMode::Normal: timeout_ = NormalTimeout; break;
                    case BlinkMode::Fast:   timeout_ = FastTimeout; break;
                }
                state_ = ((unsigned char)mode | StateMask);
                digitalWrite(LED, HIGH);
            }
            void process(int elapse)
            {
                switch (state_ & ModeMask)
                {
                    case BlinkMode::Flat:
                        return;
                        break;
                    case BlinkMode::Normal:
                        while (elapse > timeout_)
                        {
                            elapse -= timeout_;
                            timeout_ = NormalTimeout;
                            state_ ^= StateMask;
                        }
                        timeout_ -= elapse;
                        break;
                    case BlinkMode::Fast:
                        while (elapse > timeout_)
                        {
                            elapse -= timeout_;
                            timeout_ = FastTimeout;
                            state_ ^= StateMask;
                        }
                        timeout_ -= elapse;
                        break;
                }
                digitalWrite(LED, ((state_ & StateMask) ? HIGH : LOW));
            }

        private:
            long timeout_;

            //The MSBit indicates the current LED status
            unsigned char state_;
    };
}

#endif
