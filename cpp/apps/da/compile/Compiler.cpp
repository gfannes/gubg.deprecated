//#define GUBG_LOG
#include "da/compile/Compiler.hpp"
#include <sstream>
#include <stdlib.h>
using namespace da;
using namespace da::compile;
using namespace std;

Job::Job(Compiler &compiler):
    compiler_(compiler)
{
}

void Job::execute()
{
    MSS_BEGIN(void);
    MSS(operator()({object}, settings));
    compiler_.addObject_(object);
    MSS_END();
}

ReturnCode Job::creater_create(const Files &files, const Settings &settings) const
{
    MSS_BEGIN(ReturnCode, creater);
    string cmd;
    MSS(settings.get(cmd, Key::Command));
    L(cmd);
    MSS(::system(cmd.c_str()) == 0, CompilationFailed);
    MSS_END();
}

Compiler::Compiler():
    processor_(4)
{
    processor_.start();
}

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

Compiler::Command Compiler::command(const ObjectFile &obj, const SourceFile &src)
{
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
    return cmd.str();
}
ReturnCode Compiler::operator()(const ObjectFile &obj, const SourceFile &src, Headers headers)
{
    MSS_BEGIN(ReturnCode, call);

    const bool useCache = true;
    if (useCache)
    {
        auto job = Job::Ptr(new Job(*this));
        job->setCache(cache_);
        job->object = obj;
        job->settings.set(Job::Key::Object, obj.name());
        {
            gubg::hash::MD5 md5;
            string content;
            MSS(gubg::file::read(content, src));
            md5 << content;
            for (auto h: headers)
            {
                MSS(read(content, h->file()));
                md5 << content;
            }
            job->settings.set(Job::Key::DependencyMD5, md5.hash_hex());
            job->settings.set(Job::Key::Command, command(obj, src));
        }
        processor_ << job;
    }
    else
    {
        //Execute the compilation command
        auto cmd = command(obj, src);
        verbose(cmd);
        MSS(::system(cmd.c_str()) == 0, CompilationFailed);

        //Append the object file to objectFiles_
        addObject_(obj);
    }

    MSS_END();
}
void Compiler::addObject_(const ObjectFile &obj)
{
    std::lock_guard<std::mutex> lock(mutex_);
    objectFiles_.push_back(obj);
}
const ObjectFiles &Compiler::objectFiles()
{
    processor_.stop();
    return objectFiles_;
}
