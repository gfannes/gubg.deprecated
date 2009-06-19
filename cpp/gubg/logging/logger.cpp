#include "logger.hpp"

namespace gubg
{
    std::ofstream &logger()
    {
	static std::ofstream *sStream = 0;
	if (0 == sStream)
	{
	    sStream = new std::ofstream("log.txt", std::ios_base::out | std::ios_base::trunc | std::ios_base::ate);
	}
	return *sStream;
    }
}

#ifdef UNIT_TEST
using namespace std;
int main()
{
    gubg::logger() << "Test" << endl;
    return 0;
}
#endif
