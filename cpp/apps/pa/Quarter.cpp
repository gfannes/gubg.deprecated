#include "pa/Quarter.hpp"
#include "pa/Planner.hpp"
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <cassert>
using namespace pa;
using namespace std;

#define GUBG_MODULE "Quarter"
#include "gubg/log/begin.hpp"
void stream(ostream &os, Planner &planner)
{
	os << "Quarters" << endl;
	const auto quarters = gubg::planning::quarters(gubg::planning::today(), 3);
	for (const auto &quarter: quarters)
	{
		auto b = quarter.begin();
		auto e = quarter.end();
		--e;
		os << endl << "Quarter: " << *b << " " << *e << " (" << quarter.size() << "days)" << endl;
		planner.planning.overview(os, quarter);
	}

	vector<gubg::planning::Worker> workers = {"gfa", "wba"};
	for (const auto &worker: workers)
	{
		os << endl << worker << endl;
		for (const auto &quarter: quarters)
		{
			auto b = quarter.begin();
			auto e = quarter.end();
			--e;
			os << "Quarter: " << *b << " " << *e << " (" << quarter.size() << "days)" << endl;
			planner.planning.overviewForWorker(os, worker, quarter, 2);
		}
	}
}
pa::ReturnCode Quarter::execute(const Options &options)
{
	MSS_BEGIN(ReturnCode, "Planning");

	using namespace gubg::planning;
	Planner planner(options.category);
	MSS(planner.run());
	//planner.root->stream(cout);
	stream(cout, planner);
	if (!options.output.name().empty())
	{
		gubg::file::File fn(options.output.name());
		fn.setExtension("txt");
		ofstream fo(fn.name());
		stream(fo, planner);
	}

	MSS_END();
}
#include "gubg/log/end.hpp"
