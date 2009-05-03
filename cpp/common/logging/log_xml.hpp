#ifndef gubg_log_xml_hpp
#define gubg_log_xml_hpp

#include "log.hpp"

namespace gubg
{
    class XMLOutput: public Log::Output
    {
	virtual void newLevel(std::string &tag);
	virtual void closeLevel();
	virtual void newLine();
	virtual void closeLine();
	virtual Output &operator<<(const std::string &str);
    }
}

#endif
