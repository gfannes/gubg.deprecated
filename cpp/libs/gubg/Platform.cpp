#include "gubg/Platform.hpp"

#ifdef GUBG_LINUX
#include <stdlib.h>
#include <unistd.h>
#endif
#ifdef GUBG_WIN32
#endif

using namespace std;

namespace gubg
{
	bool spawn(const string &command)
	{
#ifdef GUBG_LINUX
		switch (fork())
		{
			case -1:
				//Something went wrong
				return false;
				break;
			case 0:
				//Child process
				execl("/bin/sh", "sh", "-c", command.c_str(), (char *)0);
				break;
			default:
				//Parent process, continue without waiting
				return true;
				break;
		}
#endif
		return false;
	}

	string escapeForCLI(const string &str, Quotes quotes)
	{
		string res;
		switch (quotes)
		{
			case Quotes::Add:
				res = "\"" + str + "\"";
				break;
		}
		return res;
	}
}
