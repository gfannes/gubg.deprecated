#ifndef gubg_osi_Framer_hpp
#define gubg_osi_Framer_hpp

#include "gubg/osi/Codes.hpp"

namespace gubg
{
    namespace osi
    {
        template <typename Receiver, typename Configuration>
            class Framer_crtp: public Configuration::Physical, public Configuration::Protocol
            {
                public:
                    ReturnCode process()
                    {
                        MSS_BEGIN(ReturnCode);
                        if (out_.empty())
                        {
                            if (mss::isOK(receiver_().framer_getNextFrameToSend(out_)))
                            {
                                outIt_ = out_.begin();
                                MSS(Configuration::Protocol::sendStartOfFrame(physical_()));
                            }
                        }
                        while (!out_.empty())
                        {
                            if (outIt_ == out_.end())
                            {
                                out_.clear();
                                MSS(Configuration::Protocol::sendEndOfFrame(physical_()));
                            }
                            else
                                MSS(Configuration::Protocol::sendByte(physical_(), *outIt_++));
                        }
                        MSS_END();
                    }
                private:
                    typedef typename Configuration::Frame Frame;
                    typedef typename Configuration::Frame::const_iterator FrameIt;
                    typedef typename Configuration::Physical Physical;

                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                    Physical &physical_(){return static_cast<Physical&>(*this);}

                    Frame in_;
                    Frame out_;
                    FrameIt outIt_;
            };
    }
}

#endif
