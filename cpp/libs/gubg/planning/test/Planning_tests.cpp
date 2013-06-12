#include "gubg/planning/Planning.hpp"
#include <iostream>
using namespace gubg::planning;
using namespace std;

int main()
{
	Planning planning;
	planning.addWorker("gfa", 0.8);
	planning.addWorker("wba", 0.5);
	for (auto d: workDays(90))
		planning.addDay(d);
	for (auto d: dayRange(Day(2013, 7, 4), Day(2013, 7, 20)))
		planning.absence("gfa", d);
	cout << planning;
	return 0;
}
