#include "Ring.hpp"
#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/mss.hpp"
#include "gubg/chrono/Stopwatch.hpp"
#include <thread>
using namespace std;

int main()
{
    MSS_BEGIN(int, main);

    Ring<10> ring;
    gubg::chrono::Stopwatch<> sw;
    while (sw.mark().total_elapse() < chrono::seconds(2))
    {
        ring.process(sw);
        LOG_M(ring);
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    LOG_M("Elapse: " << sw.total_elapse().count());

    MSS_END();
}
