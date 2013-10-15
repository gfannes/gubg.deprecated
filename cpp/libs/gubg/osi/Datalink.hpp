#ifndef HEADER_gubg_osi_Datalink_hpp_ALREADY_INCLUDED
#define HEADER_gubg_osi_Datalink_hpp_ALREADY_INCLUDED

#include "gubg/osi/Codes.hpp"
#include "gubg/osi/Framer.hpp"
#include <string>
#include <list>

#define GUBG_MODULE "Datalink"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace osi
    {
        template <typename Configuration>
        class Datalink: public Framer_crtp<Datalink<Configuration>, Configuration>
        {
            public:
                typedef typename Configuration::Frame Frame;

                ReturnCode send(const Frame &frame)
                {
                    MSS_BEGIN(ReturnCode);
                    outFrames_.push_back(frame);
                    MSS_END();
                }
                ReturnCode receive(Frame &frame)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS(!inFrames_.empty());
                    inFrames_.front().swap(frame);
                    inFrames_.pop_front();
                    MSS_END();
                }

                ReturnCode framer_receivedFrame(const Frame &frame)
                {
                    MSS_BEGIN(ReturnCode);
                    inFrames_.push_back(frame);
                    MSS_END();
                }
                ReturnCode framer_getNextFrameToSend(Frame &frame)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS(!outFrames_.empty());
                    outFrames_.front().swap(frame);
                    outFrames_.pop_front();
                    MSS_END();
                }
            private:
                typedef std::list<Frame> Frames;
                Frames outFrames_;
                Frames inFrames_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
