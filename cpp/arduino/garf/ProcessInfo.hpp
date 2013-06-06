#ifndef HEADER_garf_ProcessInfo_hpp_ALREADY_INCLUDED
#define HEADER_garf_ProcessInfo_hpp_ALREADY_INCLUDED

namespace garf
{
    template <typename Receiver>
        class ProcessInfo_crtp
        {
            public:
                ProcessInfo_crtp():
                    count_(0),
                    elapse_(0){}
                void process(int elapse)
                {
                    ++count_;
                    elapse_ += elapse;
                    while (elapse_ > 1000)
                    {
                        elapse_ -= 1000;
                        receiver_().pi_count(count_);
                        count_ = 0;
                    }
                }
            private:
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                long count_;
                int elapse_;
        };
}

#endif
