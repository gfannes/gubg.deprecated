#include "da/link/Linker.hpp"
#include <sstream>
#include <stdlib.h>
using namespace da;
using namespace da::compile;
using namespace std;

ReturnCode Linker::operator()(const ExeFile &exe, const ObjectFiles &objects)
{
    MSS_BEGIN(ReturnCode);

    //Prepare the command to be executed
    ostringstream cmd;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cmd << "g++ -std=c++0x -g -pthread -o " << exe.name();
        for (const auto &obj: objects)
            cmd << " " << obj.name();
        for (const auto &option: settings.linkOptions)
            cmd << " " << option;
        for (const auto &libraryPath: settings.libraryPaths)
            cmd << " -L" << libraryPath.name();
        for (const auto &lib: settings.libraries)
            cmd << " -l" << lib;
    }

    //Execute the compilation command
    verbose(cmd.str());
    MSS(::system(cmd.str().c_str()) == 0, LinkingFailed);

    MSS_END();
}
