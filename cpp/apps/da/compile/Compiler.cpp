#include "da/compile/Compiler.hpp"
#include <sstream>
#include <stdlib.h>
using namespace da;
using namespace da::compile;
using namespace std;

void Compiler::addDefine(const Define &def)
{
    defines_.push_back(def);
}
void Compiler::addSetting(const Setting &setting)
{
    settings_.push_back(setting);
}
void Compiler::addIncludePath(const IncludePath &includePath)
{
    includePaths_.push_back(includePath);
}

ReturnCode Compiler::operator()(const ObjectFile &obj, const SourceFile &src)
{
    MSS_BEGIN(ReturnCode);

    //Prepare the command to be executed
    ostringstream cmd;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cmd << "g++ -std=c++0x -O3 -pthread -c " << src.name() << " -o " << obj.name();
        for (const auto &setting: settings_)
            cmd << " " << setting;
        for (const auto &def: defines_)
            cmd << " -D" << def;
        for (const auto &includePath: includePaths_)
            cmd << " -I" << includePath.name();
    }

    //Execute the compilation command
    verbose(cmd.str());
    MSS(::system(cmd.str().c_str()) == 0, CompilationFailed);

    //Append the object file to objectFiles_
    {
        std::lock_guard<std::mutex> lock(mutex_);
        objectFiles_.push_back(obj);
    }

    MSS_END();
}
