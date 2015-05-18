#include "fff/agents/Runner.hpp"
#include "fff/Execute.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/chrono/Stopwatch.hpp"
#include <sstream>
#include <ostream>
using namespace std;

#define GUBG_MODULE "Runner"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 
	gubg::bbs::ReturnCode Runner::process(gubg::bbs::Board &board)
	{
		MSS_BEGIN(gubg::bbs::ReturnCode);

		auto tvs = board.getFrom(ix_);
		ix_ += tvs.size();

		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.tag == "start")
			{
				if (tv.value == "norun")
					do_run_ = false;
			}
			else if (tv.tag == "run.argument")
			{
                args_.push_back(tv.value);
			}
			else if (tv.tag == "c++.executable")
			{
				if (do_run_)
				{
					ostringstream command; command << tv.value;
                    for (auto arg: args_)
                        command << " " << arg;
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
