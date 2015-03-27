#include "fff/agents/Runner.hpp"
#include "fff/Board.hpp"
#include "fff/Execute.hpp"
#include "gubg/chrono/Stopwatch.hpp"
#include <sstream>
#include <ostream>
using namespace std;

#define GUBG_MODULE "Runner"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 
	ReturnCode Runner::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		auto tvs = board.getFrom(ix_);
		ix_ += tvs.size();

		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.tag == Tag("start"))
			{
				if (tv.value.as_string() == "norun")
					do_run_ = false;
			}
			else if (tv.tag == Tag("c++", "executable"))
			{
				if (do_run_)
				{
					ostringstream command; command << tv.value.as_file();
					cout << string(50, '*') << " " << command.str() << endl;
					gubg::chrono::Stopwatch<> sw;
					MSS(execute(command.str()));
					sw.mark();
					cout << string(50, '*') << " duration: " << sw.total_elapse_hr() << endl;
				}
			}
		}

		MSS_END();
	}
} } 
#include "gubg/log/end.hpp"
