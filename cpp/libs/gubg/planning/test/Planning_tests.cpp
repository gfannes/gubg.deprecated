#include "gubg/planning/Planning.hpp"
#include <iostream>
using namespace gubg::planning;
using namespace std;

int main()
{
	Planning planning;
	planning.resources.addWorker("gfa", 0.8);
	planning.resources.addWorker("wba", 0.5);
	for (auto d: workDays(90))
		planning.resources.addDay(d);
	for (auto d: dayRange(Day(2013, 7, 4), Day(2013, 7, 20)))
		planning.resources.absence("gfa", d);

	Workers workers;
	workers.push_back("gfa");
	workers.push_back("wba");
	planning.getLine("blits_l").setMaxSweatPerDay(2.0).setWorkers(workers);
	planning.getLine("blits_l").addTask(Task("a", 2.0));
	planning.getLine("blits_l").addTask(Task("b", 0.5));
	planning.getLine("blits_l").addTask(Task("c", 1.0));
	cout << planning.getLine("blits_l");
	Task task("d", 1.0);
	task.stop = Day(2013, 5, 23);
	planning.getLine("blits_l").addTask(task);
	planning.getLine("blits_l").addTask(Task("e", 1.0));
	planning.getLine("blits_l").addTask(Task("f", 1.0));
	cout << planning.getLine("blits_l");

	planning.run();
	cout << planning.getLine("blits_l");
	cout << planning.resources;
	return 0;
}
