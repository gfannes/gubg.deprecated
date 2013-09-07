#ifndef HEADER_gubg_logging_old_Primitives_hpp_ALREADY_INCLUDED
#define HEADER_gubg_logging_old_Primitives_hpp_ALREADY_INCLUDED

#include "logging/Log.hpp"

#include <fstream>

namespace gubg
{
    class COutPrimitive: public Log::Output::Primitive
    {
    public:
        virtual ~COutPrimitive();

        virtual void write(const std::string &str);
    };

    class FilePrimitive: public Log::Output::Primitive
    {
    public:
        FilePrimitive(const std::string &fileName);
         virtual ~FilePrimitive();

        virtual void write(const std::string &str);

    private:
        std::ofstream _outputFile;
    };
}

#endif
