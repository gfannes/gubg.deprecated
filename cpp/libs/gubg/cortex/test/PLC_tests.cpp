#include "gubg/cortex/PLC.hpp"
using namespace gubg::cortex;
using namespace std::chrono;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    S();
    PLC plc(seconds(2));
    plc.add(milliseconds(0), Value(3.0, 0.5));
    plc.add(milliseconds(10), Value(3.0, 0.5));
    L(plc.value(milliseconds(10), milliseconds(10)));
    return 0;
}
#include "gubg/log/end.hpp"
