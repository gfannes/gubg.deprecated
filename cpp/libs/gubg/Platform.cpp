#include "gubg/Platform.hpp"
#include <fstream>

#ifdef GUBG_LINUX
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif
#ifdef GUBG_WIN32
#include "Windows.h"
#include <unistd.h>
#include <cstdlib>
#endif

using namespace std;

#define GUBG_MODULE "Platform"
#include "gubg/log/begin.hpp"
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
#ifdef GUBG_WIN32
		std::system(command.c_str());
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

    ProcessId processId()
    {
        static const ProcessId pid = ::getpid();
        return pid;
    }
    ThreadId threadId()
    {
        //We cannot use std::this_thread::get_id(), that is to be used for comparison only, but is not the id reported by the OS
#ifdef GUBG_LINUX
        thread_local const ThreadId tid = ::syscall(SYS_gettid);
#endif
#ifdef GUBG_WIN32
        thread_local const ThreadId tid = ::GetCurrentThreadId();
#endif
        return tid;
    }
}
#include "gubg/log/end.hpp"
