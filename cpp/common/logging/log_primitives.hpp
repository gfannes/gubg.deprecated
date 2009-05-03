#ifndef gubg_log_primitives_hpp
#define gubg_log_primitives_hpp

#include "log.hpp"

namespace gubg
{
    class COutPrimitive: public Log::Output::Primitive
    {
    public:
	virtual void write(const std::string &str);
    };

    class FilePrimitive: public Log::Output::Primitive
    {
    public:
	FilePrimitive(const std::string &fileName);

	virtual void write(const std::string &str);
    };
}

#endif
