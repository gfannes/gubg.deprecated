#ifndef HEADER_garf_OOStatus_hpp_ALREADY_INCLUDED
#define HEADER_garf_OOStatus_hpp_ALREADY_INCLUDED

namespace garf
{
    template <typename Receiver, long Timeout>
        class OOStatus_crtp
        {
            public:
                OOStatus_crtp():
                    isOnline_(false),
                    timeout_(Timeout){}

                //Call this in setup to make sure Receiver will get the oostatus_offline() message
                void setup()
                {
                    isOnline_ = false;
                    receiver_().oostatus_offline();
                }

                //Will send oostatus_offline() when we timeout and become offline
                void process(int elapse)
                {
                    if (!isOnline_)
                        //We are already offline, further processing doesn't matter anymore
                        return;
                    if (elapse <= timeout_)
                    {
                        //Timeout is getting closer...
                        timeout_ -= elapse;
                        return;
                    }
                    //We timed-out and will become offline
                    timeout_ = 0;
                    isOnline_ = false;
                    receiver_().oostatus_offline();
                }

                //Indicate that we received an alive message
                //This will send oostatus_online() to Receiver when it comes online again
                void indicateOnline()
                {
                    timeout_ = Timeout;
                    if (isOnline_)
                        return;
                    isOnline_ = true;
                    receiver_().oostatus_online();
                }

            private:
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                bool isOnline_;
                long timeout_;
        };
}

#endif
