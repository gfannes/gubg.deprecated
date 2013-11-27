#include "gubg/planning/Planning.hpp"
#include <iostream>
using namespace gubg::planning;
using namespace std;

int main()
{
	Planning planning;
	planning.addWorker("gfa", 0.8);
	planning.addWorker("wba", 0.5);
	for (auto d: workDays(10))
		planning.addDay(d);
	for (auto d: dayRange(Day(2013, 7, 4), Day(2013, 7, 20)))
		planning.absence("gfa", d);
	cout << planning;

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
    Workers workers;
    workers.push_back("gfa");
    ch11->setWorkers(workers);
    workers.push_back("wba");
    root->setWorkers(workers);

    planning.plan(*root);

	cout << planning;

	return 0;
}
