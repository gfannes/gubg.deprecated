#include "gubg/Async.hpp"
#include "gubg/l.hpp"
using namespace std;
using namespace gubg;

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
            L("i: " << i);
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    };
}

int main()
{
    A a;
    for (int i = 0; i < 100; ++i)
        a.addJob(i);
    this_thread::sleep_for(chrono::seconds(1));
    return 0;
}
