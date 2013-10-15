#include "gubg/osi/Physical.hpp"
#include "gubg/osi/Datalink.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    using gubg::osi::ReturnCode;
    struct Protocol_
    {
        template <typename Physical>
        ReturnCode sendStartOfFrame(Physical &ph)
        {
            MSS_BEGIN(ReturnCode);
            ph.sendByte("<");
            MSS_L(Proceed);
            MSS_END();
        }
        template <typename Physical>
        ReturnCode sendEndOfFrame(Physical &ph)
        {
            MSS_BEGIN(ReturnCode);
            ph.sendByte(">");
            MSS_L(Proceed);
            MSS_END();
        }
        template <typename Physical, typename Byte>
        ReturnCode sendByte(Physical &ph, Byte byte)
        {
            MSS_BEGIN(ReturnCode);
            ph.sendByte(byte);
            MSS_L(Proceed);
            MSS_END();
        }
    };
    struct Configuration
    {
        typedef std::string Frame;
        typedef gubg::osi::StdioPhysical Physical;
        typedef Protocol_ Protocol;
    };
    typedef gubg::osi::Datalink<Configuration> Datalink;
}

int main()
{
    S();
    Datalink datalink;
    datalink.send("abc");
    for (auto i = 0; i < 10; ++i)
    {
        datalink.process();
        L("|");
    }
    return 0;
}
#include "gubg/log/end.hpp"
