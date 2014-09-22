#include "fff/Execute.hpp"
#include <iostream>
#include <cstdlib>
using namespace std;

#define GUBG_MODULE_ "Execute"
#include "gubg/log/begin.hpp"
namespace fff { 
	ReturnCode execute(string command)
	{
		MSS_BEGIN(ReturnCode);
		L("Executing \"" << command << "\" ...");
		cout << command << endl;
		const auto ret = std::system(command.c_str());
		L(" ... done (ret=" << ret << ")");
		if (ret != 0)
		{
			cerr << "Failed to execute \"" << command << "\"" << endl;
			//exit(-1);
		}
		MSS(ret == 0);
		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
