#include "fff/Compiler.hpp"

#define GUBG_MODULE "Compiler"
#include "gubg/log/begin.hpp"
namespace fff { 

    class GCC: public compiler::Interface
    {
        public:
            void stream_Command(Stream &stream) const override { stream << "g++ -std=c++0x -c"; }
            void stream_Source(Stream &stream, const gubg::file::File &src) const override { stream << " " << src; }
            void stream_Object(Stream &stream, const gubg::file::File &obj) const override { stream << " -o " << obj; }
            void stream_IncludePath(Stream &stream, const gubg::file::File &ip) const override { stream << " -I" << ip; }
            void stream_Define(Stream &stream, const std::string &def) const override { stream << " -D" << def; }

            void debug() override
            {
                options.push_back("-g");
                defines.push_back("DEBUG");
            }
    };
    class CLang: public GCC
    {
        public:
            void stream_Command(Stream &stream) const override { stream << "clang -c"; }
            //All the rest is the same as gcc
    };
    class MSC: public compiler::Interface
    {
        public:
            void stream_Command(Stream &stream) const override { stream << "cl -c"; }
            void stream_Source(Stream &stream, const gubg::file::File &src) const override { stream << " " << src; }
            void stream_Object(Stream &stream, const gubg::file::File &obj) const override { stream << " /Fo" << obj; }
            void stream_IncludePath(Stream &stream, const gubg::file::File &ip) const override { stream << " /I" << ip; }
            void stream_Define(Stream &stream, const std::string &def) const override { stream << " /D" << def; }

            void debug() override
            {
                options.push_back("/Zi");
                defines.push_back("DEBUG");
            }
    };

    Compiler::Compiler(compiler::Type type)
    {
        switch (type)
        {
            case compiler::Type::GCC: itf_.reset(new GCC); break;
            case compiler::Type::MSC: itf_.reset(new MSC); break;
            case compiler::Type::CLang: itf_.reset(new CLang); break;
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
        if (false) {}
        else if (option == "debug") { itf_->debug(); }
        else
        {
            MSS_L(UnknownOption);
        }
        MSS_END();
    }

    ReturnCode Compiler::compile(std::string &cli, const gubg::file::File &src, const gubg::file::File &obj) const
    {
        MSS_BEGIN(ReturnCode);
        MSS((bool)itf_);
        compiler::Interface::Stream stream;
        itf_->stream_Command(stream);
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
