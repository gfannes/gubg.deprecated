#ifndef HEADER_garf_Blinker_hpp_ALREADY_INCLUDED
#define HEADER_garf_Blinker_hpp_ALREADY_INCLUDED

#include "Arduino.h"

namespace garf
{
    enum class BlinkMode: unsigned char { On, Off, Normal, Fast};

    template <long NormalTimeout_, int LED_ = 13>
    class Blinker
    {
        private:
            static const unsigned char InitMask = 0x80;
            static const unsigned char StateMask = 0x40;
            static const unsigned char ModeMask = 0x3f;

        public:
            static const long NormalTimeout = NormalTimeout_;
            static const long FastTimeout = NormalTimeout_/4;
            static const long LED = LED_;

            Blinker():
                state_((unsigned char)BlinkMode::On | StateMask | InitMask),
                timeout_(0)
            {
            }

            void set(BlinkMode mode)
            {
                doInitialization_();

                const BlinkMode currentMode = BlinkMode(state_ & ModeMask);
                if (mode == currentMode)
                    return;
                bool on = true;
                switch (mode)
                {
                    case BlinkMode::On:     timeout_ = 0;             on = true; break;
                    case BlinkMode::Off:    timeout_ = 0;             on = false; break;
                    case BlinkMode::Normal: timeout_ = NormalTimeout; on = true; break;
                    case BlinkMode::Fast:   timeout_ = FastTimeout;   on = true; break;
                }
                if (on)
                    state_ = ((unsigned char)mode | StateMask);
                else
                    state_ = (unsigned char)mode;
                digitalWrite(LED, on ? HIGH : LOW);
            }

            void process(unsigned int elapse)
            {
                doInitialization_();

                switch ((BlinkMode)(state_ & ModeMask))
                {
                    case BlinkMode::On:
                    case BlinkMode::Off:
                        return; break;
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
            void doInitialization_()
            {
                if (state_ & InitMask)
                {
                    //We delay initialization beyond the ctor to make sure
                    //normal arduino setup was done
                    pinMode(LED, OUTPUT);
                    digitalWrite(LED, HIGH);
                    state_ ^= InitMask;
                }
            }

            unsigned long timeout_;

            //The MSBit indicates the current LED status
            unsigned char state_;
    };
}

#endif
