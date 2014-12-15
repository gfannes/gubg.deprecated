#ifndef HEADER_fff_Linker_hpp_ALREADY_INCLUDED
#define HEADER_fff_Linker_hpp_ALREADY_INCLUDED

#include "fff/Codes.hpp"
#include "gubg/file/File.hpp"
#include <memory>
#include <sstream>

namespace fff { 

    namespace linker { 
        enum class Vendor
        {
            GCC, MSC, CLang,
        };
        enum class OutputType
        {
            Exe, Shared,
        };
        class Interface
        {
            public:
                OutputType outputType;
                typedef std::vector<gubg::file::File> Objects;
                Objects objects;
                typedef std::vector<gubg::file::File> LibraryPaths;
                LibraryPaths libraryPaths;
                typedef std::vector<std::string> Libraries;
                Libraries libraries;
                typedef std::vector<std::string> Options;
                Options options;

                typedef std::ostringstream Stream;
                virtual void stream_Command(Stream &) const = 0;
                virtual void stream_Output(Stream &, const gubg::file::File &) const = 0;
                virtual void stream_Object(Stream &, const gubg::file::File &) const = 0;
                virtual void stream_LibraryPath(Stream &, const gubg::file::File &) const = 0;
                virtual void stream_Library(Stream &, const std::string &) const = 0;

                virtual bool setOption(const std::string &option) = 0;
        };
    } 

    class Linker
    {
        public:
            Linker(linker::Vendor, linker::OutputType);

            ReturnCode addObject(const gubg::file::File &);
            ReturnCode addObject(const std::string &obj) {return addObject(gubg::file::File(obj));}

            ReturnCode addLibraryPath(const gubg::file::File &);
            ReturnCode addLibraryPath(const std::string &str) {return addLibraryPath(gubg::file::File(str));}
            ReturnCode addLibrary(const std::string &);

            ReturnCode addOption(const std::string &);

            ReturnCode link(std::string &cli, const gubg::file::File &exe) const;

        private:
            std::unique_ptr<linker::Interface> itf_;
    };
} 

#endif
