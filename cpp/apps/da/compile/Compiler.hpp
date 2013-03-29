#ifndef HEADER_da_compile_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_da_compile_Compiler_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/Types.hpp"
#include "da/build/Headers.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Creater.hpp"
#include "gubg/Settings.hpp"
#include "gubg/threading/Processor.hpp"
#include "gubg/Verbose.hpp"
#include <string>
#include <mutex>

namespace da
{
    namespace compile
    {
        typedef gubg::file::File SourceFile;
        typedef gubg::file::File ObjectFile;
        typedef std::list<ObjectFile> ObjectFiles;

        class Compiler;

        class Job: public gubg::file::Creater_crtp<Job>
        {
            public:
                enum class Key {Object, DependencyMD5, Command};
                typedef gubg::Settings<Key> Settings;
                typedef std::shared_ptr<Job> Ptr;

                Job(Compiler &compiler);

                void execute();

                ReturnCode creater_create(const Files &files, const Settings &settings) const;

                ObjectFile object;
                Settings settings;

            private:
                Compiler &compiler_;
        };

        class Compiler: public gubg::Verbose<true>
        {
            public:
                Compiler(ExeType);

                CompileSettings settings;

                typedef std::string Command;
                Command command(const ObjectFile &, const SourceFile &);
                ReturnCode operator()(const ObjectFile &, const SourceFile &, Headers headers);

                size_t nrFailures();
                void addFailure();

                const ObjectFiles &objectFiles();

                void setCache(gubg::file::File cache){cache_ = cache;}

            private:
                friend class Job;
                void addObject_(const ObjectFile &);

                ObjectFiles objectFiles_;
                typedef gubg::threading::Processor<Job> Processor;
                Processor processor_;
                gubg::file::File cache_;
                size_t nrFailures_;

                std::mutex mutex_;
                const ExeType exeType_;
        };
    }
}

#endif
