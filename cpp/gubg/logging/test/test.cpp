#define GUBG_LOG
//#define LOG_LEVEL Info
#include "logging/Log.hpp"

int main()
{
    LOG_S(main, "This is the main scope: ");
    LOG_M("Another message in the main scope");
    LOG_M_(Fatal, "Fatal message")
    LOG_M_(Info, "Info message")
    LOG_M_(Debug, "Debug message")
    {
        LOG_S(level1, "Level1 scope starts");
        LOG_M("And this one has several messages");
        LOG_M("Like two...");
    }
    LOG_M("And again at the main scope");
}
