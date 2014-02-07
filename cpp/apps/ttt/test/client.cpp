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
                Scope s2(Msg(1) << "Thread work " << nr_++ << " " << name_);
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

    private:
        int nr_ = 0;
        const string name_;
        thread thread_;
};

int main()
{
    Scope s1(Msg(0) << "Main thread");
#if 1
    Thread a("a");
    Thread b("b");
    Thread c("c");
#endif
    int nr = 0;
    while (true)
    {
        Scope s2(Msg(1) << "Main work " << nr++);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    return 0;
}
