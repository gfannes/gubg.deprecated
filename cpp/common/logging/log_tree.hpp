#ifndef gubg_log_tree_hpp
#define gubg_log_tree_hpp

#include "log.hpp"

namespace gubg
{
    class TreeOutput: public Log::Output
    {
    public:
	TreeOutput(Log::Output::Primitive *primitive);

	virtual void newLevel(const std::string &fileName, unsigned int lineNr, const std::string &tag);
	virtual void closeLevel(const std::string &tag);
	virtual void newLine(const std::string &fileName, unsigned int lineNr);
	virtual void closeLine();
	virtual Output &operator<<(const std::string &str);

    private:
	static unsigned int _indentLevel;
	void indent();
    };
}

#endif