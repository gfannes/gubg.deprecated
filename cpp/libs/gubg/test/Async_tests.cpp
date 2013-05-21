#include "gubg/Async.hpp"
#include <chrono>
using namespace std;
using namespace gubg;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    class A: public Async_crtp<A, int>
    {
        public:
            A(): Async_crtp(3){}
            ~A()
            {
                //clearJobs();
                join();
            }
        void async_process(int i)
        {
            SS("i: " << i);
            this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };
}

int main()
{
    A a;
    for (int i = 0; i < 100; ++i)
        a.addJob(i);
    this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
