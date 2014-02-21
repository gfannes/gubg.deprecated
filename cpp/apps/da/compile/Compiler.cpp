#include "da/compile/Compiler.hpp"
#include "da/Arduino.hpp"
#include "gubg/env/Variables.hpp"
#include <sstream>
#include <stdlib.h>
#include <cstdlib>
#include <cassert>
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
    LLL(cmd);
    MSS(::system(cmd.c_str()) == 0, CompilationFailed);
    MSS_END();
}

namespace 
{
    int nrProcessors()
    {
        string nr_str;
        gubg::env::Variables::shell().get(nr_str, "GUBG_NUMBER_CPU");
        int nr = std::atoi(nr_str.c_str());
        if (nr <= 0)
            nr = 1;
        return nr;
    }
}
Compiler::Compiler(ExeType exeType):
    processor_(nrProcessors()),
    nrFailures_(0),
    exeType_(exeType)
{
    processor_.start();
}

Compiler::Command Compiler::command(const ObjectFile &obj, const SourceFile &src)
{
    //Prepare the command to be executed
    ostringstream cmd;
    {
        std::lock_guard<std::mutex> lock(mutex_);

		string compiler;
		{
			const auto ext = src.extension();
			if (false) {}
			else if (ext == "c")   { compiler = "gcc "; }
			else if (ext == "cpp" or ext == "cxx") { compiler = "g++ -std=c++11 -pthread "; }
			switch (exeType_)
			{
				case ExeType::Debug:   compiler += "-g "; break;
				case ExeType::Release: compiler += "-O3 "; break;
				default: assert(false); break;
			}
		}

		string buildConfig;
		switch (exeType_)
		{
			case ExeType::Debug: buildConfig = "-DGUBG_DEBUG "; break;
			case ExeType::Release: buildConfig = "-DGUBG_RELEASE "; break;
			default: assert(false); break;
		}

        switch (settings.targetPlatform)
        {
            case Any:
            case Host:
				cmd << compiler << "-c " << buildConfig;
                break;
            case Arduino:
                if (arduino::isUno())
                    cmd << "avr-g++ -std=c++0x -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=100 -D__AVR_LIBC_DEPRECATED_ENABLE__ -c ";
                else if (arduino::isMega())
                    cmd << "avr-g++ -std=c++0x -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=100 -D__AVR_LIBC_DEPRECATED_ENABLE__ -c ";
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
