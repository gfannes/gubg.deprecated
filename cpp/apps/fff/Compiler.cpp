#include "fff/Compiler.hpp"

#define GUBG_MODULE "Compiler"
#include "gubg/log/begin.hpp"
namespace fff { 

    namespace compiler { 
        class GCC: public Interface
        {
            public:
                void stream_Command(Stream &stream, Language language) const override
                {
                    switch (language)
                    {
                        case Language::Cpp: stream << "g++ -std=c++0x"; break;
                        case Language::C: stream << "gcc"; break;
                    }
                    stream << " -c";
                }
                void stream_Source(Stream &stream, const gubg::file::File &src) const override { stream << " " << src; }
                void stream_Object(Stream &stream, const gubg::file::File &obj) const override { stream << " -o " << obj; }
                void stream_IncludePath(Stream &stream, const gubg::file::File &ip) const override { stream << " -I" << ip; }
                void stream_Define(Stream &stream, const std::string &def) const override { stream << " -D" << def; }

                bool setOption(const std::string &option) override
                {
                    if (false) {}
                    else if (option == "debug")
                    {
                        options.push_back("-g");
                        defines.push_back("DEBUG");
                        defines.push_back("GUBG_DEBUG");
                    }
                    else if (option == "release")
                    {
                        options.push_back("-O3");
                        defines.push_back("NDEBUG");
                        defines.push_back("GUBG_RELEASE");
                    }
                    else if (option == "shared")
                    {
                        options.push_back("-fPIC");
                    }
                    else
                        return false;
                    return true;
                }
        };
        class CLang: public GCC
        {
            public:
                void stream_Command(Stream &stream, Language language) const override { stream << "clang -c"; }
                //All the rest is the same as gcc
        };
        class MSC: public Interface
        {
            public:
                void stream_Command(Stream &stream, Language language) const override
                {
                    switch (language)
                    {
                        case Language::Cpp: stream << "cl /FS"; break;
                        case Language::C: stream << "cl /FS /TC"; break;
                    }
                    stream << " -c";
                }
                void stream_Source(Stream &stream, const gubg::file::File &src) const override { stream << " " << src; }
                void stream_Object(Stream &stream, const gubg::file::File &obj) const override { stream << " /Fo" << obj; }
                void stream_IncludePath(Stream &stream, const gubg::file::File &ip) const override { stream << " /I" << ip; }
                void stream_Define(Stream &stream, const std::string &def) const override { stream << " /D" << def; }

                bool setOption(const std::string &option) override
                {
                    if (false) {}
                    else if (option == "debug")
                    {
                        options.push_back("/Zi");
                        defines.push_back("DEBUG");
                        defines.push_back("GUBG_DEBUG");
                    }
                    else if (option == "release")
                    {
                        options.push_back("/O3");
                        defines.push_back("NDEBUG");
                        defines.push_back("GUBG_RELEASE");
                    }
                    else if (option == "shared")
                    {
                    }
                    else
                        return false;
                    return true;
                }
        };
    } 

    Compiler::Compiler(compiler::Vendor vendor)
    {
        switch (vendor)
        {
            case compiler::Vendor::GCC: itf_.reset(new compiler::GCC); break;
            case compiler::Vendor::MSC: itf_.reset(new compiler::MSC); break;
            case compiler::Vendor::CLang: itf_.reset(new compiler::CLang); break;
        }
    }

    ReturnCode Compiler::addIncludePath(const gubg::file::File &ip)
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        itf_->includePaths.push_back(ip);
        MSS_END();
    }
    ReturnCode Compiler::addDefine(const std::string &def)
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        itf_->defines.push_back(def);
        MSS_END();
    }
    ReturnCode Compiler::addOption(const std::string &option)
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        MSS(itf_->setOption(option), UnknownOption);
        MSS_END();
    }

    ReturnCode Compiler::compile(std::string &cli, const gubg::file::File &src, const gubg::file::File &obj, compiler::Language language) const
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        compiler::Interface::Stream stream;
        itf_->stream_Command(stream, language);
        itf_->stream_Source(stream, src);
        itf_->stream_Object(stream, obj);
        for (const auto &ip: itf_->includePaths)
            itf_->stream_IncludePath(stream, ip);
        for (const auto &def: itf_->defines)
            itf_->stream_Define(stream, def);
        for (const auto &option: itf_->options)
            stream << " " << option;
        cli = stream.str();
        MSS_END();
    }

} 
#include "gubg/log/end.hpp"
