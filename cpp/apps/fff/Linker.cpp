#include "fff/Linker.hpp"

#define GUBG_MODULE "Linker"
#include "gubg/log/begin.hpp"
namespace fff { 

    namespace linker { 
        class GCC: public Interface
        {
            public:
                void stream_Command(Stream &stream) const override { stream << "g++ -std=c++0x"; }
                void stream_Output(Stream &stream, const gubg::file::File &src) const override { stream << " -o " << src; }
                void stream_Object(Stream &stream, const gubg::file::File &obj) const override { stream << " " << obj; }
                void stream_LibraryPath(Stream &stream, const gubg::file::File &lp) const override { stream << " -L" << lp; }
                void stream_Library(Stream &stream, const std::string &lib) const override { stream << " -l" << lib; }

                bool setOption(const std::string &option) override
                {
                    if (false) {}
                    else if (option == "debug")
                    {
                        options.push_back("-g");
                    }
                    else if (option == "release")
                    {
                    }
                    else if (option == "thread")
                    {
                        options.push_back("-pthread");
                    }
                    else
                        return false;
                    return true;
                }
        };
        class CLang: public GCC
        {
            public:
                void stream_Command(Stream &stream) const override { stream << "clang"; }
                //All the rest is the same as gcc
        };
        class MSC: public Interface
        {
            public:
                void stream_Command(Stream &stream) const override { stream << "link /nologo"; }
                void stream_Output(Stream &stream, const gubg::file::File &src) const override { stream << " /OUT:" << src; }
                void stream_Object(Stream &stream, const gubg::file::File &obj) const override { stream << " " << obj; }
                void stream_LibraryPath(Stream &stream, const gubg::file::File &lp) const override { stream << " /LIBPATH:" << lp; }
                void stream_Library(Stream &stream, const std::string &lib) const override { stream << " " << lib << ".lib"; }

                bool setOption(const std::string &option) override
                {
                    if (false) {}
                    else if (option == "debug")
                    {
                        options.push_back("/DEBUG");
                    }
                    else if (option == "release")
                    {
                    }
                    else if (option == "thread")
                    {
                    }
                    else
                        return false;
                    return true;
                }
        };
    } 

    Linker::Linker(linker::Vendor vendor, linker::OutputType ot)
    {
        switch (vendor)
        {
            case linker::Vendor::GCC: itf_.reset(new linker::GCC); break;
            case linker::Vendor::MSC: itf_.reset(new linker::MSC); break;
            case linker::Vendor::CLang: itf_.reset(new linker::CLang); break;
        }
        itf_->outputType = ot;
    }

    ReturnCode Linker::addObject(const gubg::file::File &obj)
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        itf_->objects.push_back(obj);
        MSS_END();
    }
    ReturnCode Linker::addLibraryPath(const gubg::file::File &lp)
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        itf_->libraryPaths.push_back(lp);
        MSS_END();
    }
    ReturnCode Linker::addLibrary(const std::string &lib)
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        itf_->libraries.push_back(lib);
        MSS_END();
    }
    ReturnCode Linker::addOption(const std::string &option)
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        MSS(itf_->setOption(option), UnknownOption);
        MSS_END();
    }

    ReturnCode Linker::link(std::string &cli, const gubg::file::File &exe) const
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        linker::Interface::Stream stream;
        itf_->stream_Command(stream);
        itf_->stream_Output(stream, exe);
        for (const auto &obj: itf_->objects)
            itf_->stream_Object(stream, obj);
        for (const auto &lp: itf_->libraryPaths)
            itf_->stream_LibraryPath(stream, lp);
        for (const auto &lib: itf_->libraries)
            itf_->stream_Library(stream, lib);
        for (const auto &option: itf_->options)
            stream << " " << option;
        cli = stream.str();
        MSS_END();
    }

} 
#include "gubg/log/end.hpp"
