#include "player/App.hpp"

#define GUBG_MODULE_ "main"
#include "gubg/log/begin.hpp"
int main()
{
    S();
    player::App app;
    while (app()) { }
    return 0;
}
#include "gubg/log/end.hpp"
