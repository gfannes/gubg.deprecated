#include "da/compile/Compiler.hpp"
#include "da/Arduino.hpp"
#include <sstream>
#include <stdlib.h>
using namespace da;
using namespace da::compile;
using namespace std;

#define GUBG_MODULE "Compiler"
#include "gubg/log/begin.hpp"
Job::Job(Compiler &compiler):
    compiler_(compiler)
{
}

void Job::execute()
{
    MSS_BEGIN(void);
    auto rc = operator()({object}, settings);
    if (!gubg::mss::isOK(rc))
    {
        compiler_.addFailure();
        MSS(rc);
    }
    compiler_.addObject_(object);
    MSS_END();
}

ReturnCode Job::creater_create(const Files &files, const Settings &settings) const
{
    MSS_BEGIN(ReturnCode);
    string cmd;
    MSS(settings.get(cmd, Key::Command));
    L(cmd);
    MSS(::system(cmd.c_str()) == 0, CompilationFailed);
    MSS_END();
}

Compiler::Compiler():
    processor_(4),
    nrFailures_(0)
{
    processor_.start();
}

Compiler::Command Compiler::command(const ObjectFile &obj, const SourceFile &src)
{
    //Prepare the command to be executed
    ostringstream cmd;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        switch (settings.targetPlatform)
        {
            case Any:
            case Host:
                cmd << "g++ -std=c++0x -O3 -pthread -c -DGUBG_DEBUG ";
                break;
            case Arduino:
                if (arduino::isUno())
                    cmd << "avr-g++ -std=c++0x -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=100 -c ";
                else if (arduino::isMega())
                    cmd << "avr-g++ -std=c++0x -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=100 -c ";
                break;
            default:
                cmd << "UNKNOWN TARGET PLATFORM ";
                break;
        }
        cmd << src.name() << " -o " << obj.name();
        for (const auto &option: settings.compileOptions)
            cmd << " " << option;
        for (const auto &def: settings.defines)
            cmd << " -D" << def;
        for (const auto &includePath: settings.includePaths)
            cmd << " -I" << includePath.name();
    }
    return cmd.str();
}
ReturnCode Compiler::operator()(const ObjectFile &obj, const SourceFile &src, Headers headers)
{
    MSS_BEGIN(ReturnCode);

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
void Compiler::addFailure()
{
    std::lock_guard<std::mutex> lock(mutex_);
    ++nrFailures_;
}
size_t Compiler::nrFailures()
{
    processor_.stop();
    return nrFailures_;
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
