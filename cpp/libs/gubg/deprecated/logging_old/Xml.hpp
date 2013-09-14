#ifndef HEADER_gubg_deprecated_logging_old_Xml_hpp_ALREADY_INCLUDED
#define HEADER_gubg_deprecated_logging_old_Xml_hpp_ALREADY_INCLUDED

#include "logging/Log.hpp"

namespace gubg
{
    class XMLOutput: public Log::Output
    {
    public:
        XMLOutput(Log::Output::Primitive *primitive);

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
