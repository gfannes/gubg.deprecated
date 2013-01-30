#include "gubg/Async.hpp"
#include "gubg/l.hpp"
using namespace std;
using namespace gubg;

namespace 
{
    class A: public Async_crtp<A, int>
    {
        public:
        void async_process(int i)
        {
            L("i: " << i);
        }
    };
}

int main()
{
    A a;
    a.setWorkerCount(3);
    a.addJob(123);
    this_thread::sleep_for(chrono::seconds(5));
    return 0;
}
