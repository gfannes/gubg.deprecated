//#define GUBG_LOG
#include "gubg/threading/Processor.hpp"
#include "gubg/Testing.hpp"
#include "gubg/l.hpp"
#include <chrono>
using namespace std::chrono;

namespace 
{
    std::mutex lm;
#define LL(msg) lm.lock(); L(msg); lm.unlock()
    class Job
    {
        public:
            typedef std::shared_ptr<Job> Ptr;
            template <typename Duration>
                static Ptr create(Duration duration){return Ptr(new Job(duration));}
            template <typename Duration>
                Job(Duration duration):duration_(duration){}

            void execute()
            {
                MSS_BEGIN(void, execute);
                LL("Starting job " << duration_.count());
                std::this_thread::sleep_for(duration_);
                LL("         job " << duration_.count() << " is done");
                MSS_END();
            }

        private:
            milliseconds duration_;
    };
    typedef gubg::threading::Processor<Job> Processor;
}

int main()
{
    TEST_TAG(Processor);
    Processor processor(5);
    for (auto i = 0; i < 20; ++i)
        processor << Job::create(milliseconds(100*i));
    TEST_KO(processor.stop());
    TEST_OK(processor.start());
    TEST_KO(processor.start());

    std::this_thread::sleep_for(milliseconds(100));

    TEST_OK(processor.stop());
    TEST_KO(processor.stop());
    return 0;
}
