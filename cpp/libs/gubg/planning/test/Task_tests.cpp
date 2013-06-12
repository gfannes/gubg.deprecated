#include "gubg/planning/Task.hpp"
using namespace gubg::planning;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    S();
    Task::Ptr root = Task::create("root");
    auto ch1 = root->addChild("ch1");
    auto ch11 = ch1->addChild("ch11");
    auto ch12 = ch1->addChild("ch12");
    auto ch2 = root->addChild("ch2");
    root->setDeadline(Day(2013, 12, 31));
    ch1->setDeadline(Day(2013, 6, 12));
    ch11->setSweat(1);
    ch12->setSweat(1.5);
    ch2->setSweat(2);
    L(*root);
    root->distributeDeadlines();
    auto tpd = root->tasksPerDeadline();
    L(*root);
    for (auto t: tpd)
        L(*t.second);
    return 0;
}
#include "gubg/log/end.hpp"
