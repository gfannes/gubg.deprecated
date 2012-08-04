#ifndef garf_Status_hpp
#define garf_Status_hpp

namespace garf
{
    template <long Timeout>
    class Status
    {
        public:
            Status():
                state_(false),
                toggleTimeout_(Timeout)
            {
                pinMode(LED, OUTPUT);
            }
            void process(int elapse)
            {
                while (elapse > toggleTimeout_)
                {
                    elapse -= toggleTimeout_;
                    toggleTimeout_ = Timeout;
                    state_ = !state_;
                    digitalWrite(LED, (state_ ? HIGH : LOW));
                }
                toggleTimeout_ -= elapse;
            }
        private:
            static const int LED = 13;
            long toggleTimeout_;
            bool state_;
    };
}

#endif
