#include "gubg/trace/Scope.hpp"
#include <thread>
#include <string>
using namespace std;
using namespace gubg::trace;

class Thread
{
    public:
        Thread(const string &name): name_(name), thread_(ref(*this)) {}

        void operator()()
        {
            Scope s1(Msg(0) << "Thread " << name_);
            while (true)
            {
                Scope s2(Msg(1) << "Thread work");
                this_thread::sleep_for(chrono::milliseconds(200));
            }
        }

    private:
        const string name_;
        thread thread_;
};

int main()
{
    Scope s1(Msg(0) << "Main thread");
#if 0
    Thread a("a");
    Thread b("b");
    Thread c("c");
#endif
    while (true)
    {
        Scope s2(Msg(1) << "Main work");
        this_thread::sleep_for(chrono::milliseconds(200));
    }
    return 0;
}
