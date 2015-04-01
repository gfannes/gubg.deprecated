#ifndef HEADER_fff_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_fff_Compiler_hpp_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include "gubg/file/File.hpp"
#include <memory>
#include <sstream>

namespace fff { 

    namespace compiler { 
        enum class Vendor
        {
            GCC, MSC, CLang,
        };
        enum class Language
        {
            Cpp, C,
        };
        class Interface
        {
            public:
                typedef std::vector<gubg::file::File> IncludePaths;
                IncludePaths includePaths;
                typedef std::vector<std::string> Defines;
                Defines defines;
                typedef std::vector<std::string> Options;
                Options options;

                typedef std::ostringstream Stream;
                virtual void stream_Command(Stream &, Language) const = 0;
                virtual void stream_Source(Stream &, const gubg::file::File &) const = 0;
                virtual void stream_Object(Stream &, const gubg::file::File &) const = 0;
                virtual void stream_IncludePath(Stream &, const gubg::file::File &) const = 0;
                virtual void stream_Define(Stream &, const std::string &) const = 0;

                virtual bool setOption(const std::string &option) = 0;
        };
    } 

    class Compiler
    {
        public:
            Compiler(compiler::Vendor);
            Compiler &operator=(Compiler&&dying);

            ReturnCode addIncludePath(const gubg::file::File &);
            ReturnCode addIncludePath(const std::string &str) {return addIncludePath(gubg::file::File(str));}
            ReturnCode addDefine(const std::string &);

            ReturnCode addOption(const std::string &);

            ReturnCode compile(std::string &cli, const gubg::file::File &src, const gubg::file::File &obj, compiler::Language) const;

        private:
            std::unique_ptr<compiler::Interface> itf_;
    };
} 

#endif
