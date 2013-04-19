#define GUBG_LOG
#define LOG_LEVEL Info
#define GUBG_MODULE "Logging unit test"
#include "gubg/logging/Log.hpp"
#include "gubg/threading/InstanceCounter.hpp"
#include "boost/thread/mutex.hpp"
using namespace boost;

namespace
{
    struct Thread: gubg::InstanceCounter<Thread>
    {
        Thread():
            thread_(ref(*this)){}

        void operator()()
        {
            LOG_S(thread, "Thread starting up");
            LOG_M("And some message");
            delete this;
        }

        thread thread_;
    };
}

int main()
{
    LOG_S(main, "This is the main scope: ");
    LOG_M("Another message in the main scope");
    LOG_M_(Fatal, "Fatal message");
    LOG_M_(Info, "Info message");
    LOG_M_(Debug, "Debug message");
    {
        LOG_S(level1, "Level1 scope starts");
        LOG_M("And this one has several messages");
        LOG_M("Like two...");
    }
    LOG_M("And again at the main scope");

    {
        LOG_S_(Debug, scope1, "This message should not be printed but the scope will be opened");
        LOG_M_(Info, "This message will be printed");
    }

    {
        const int NrThreads = 100;
        LOG_S(threadLauncher, "Launching " << NrThreads << " threads");
        for (int i = 0; i < NrThreads; ++i)
            new Thread;
        while (Thread::nrInstances() > 0)
            ;
    }
}
