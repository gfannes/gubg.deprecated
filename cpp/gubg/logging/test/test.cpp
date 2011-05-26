#define GUBG_LOG
#include "logging/Log.hpp"

int main()
{
    LOG_S(main, "This is the main scope");
    LOG_M("Another message in the main scope");
    {
        LOG_S(level1, "Level1 scope starts");
        LOG_M("And this one has several messages");
        LOG_M("Like two...");
    }
    LOG_M("And again at the main scope");
}
