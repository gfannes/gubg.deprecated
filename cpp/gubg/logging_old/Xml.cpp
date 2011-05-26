#include "logging/Xml.hpp"
#include "logging/Primitives.hpp"

#include <sstream>

using namespace gubg;

unsigned int XMLOutput::_indentLevel = 0;

XMLOutput::XMLOutput(Log::Output::Primitive *primitive):
    Log::Output(primitive)
{
}

void XMLOutput::newLevel(const std::string &fileName, unsigned int lineNr, const std::string &tag)
{
    indent();
    std::ostringstream str;
    str << "<Scope tag = \"" << tag << "\">" << std::endl;
    write(str.str());
    ++_indentLevel;
}

void XMLOutput::closeLevel(const std::string &tag)
{
    --_indentLevel;
    indent();
    write("</Scope>\n");
}

void XMLOutput::newLine(const std::string &fileName, unsigned int lineNr)
{
    indent();
    std::ostringstream str;
    str << "<Line fileName = \"" << fileName << "\", lineNr = \"" << lineNr << "\">";
    write(str.str());
}

void XMLOutput::closeLine()
{
    write("</Line>\n");
}

Log::Output &XMLOutput::operator<<(const std::string &str)
{
    write(str);
}

void XMLOutput::indent()
{
    std::string id(_indentLevel*2, ' ');
    write(id);
}

#ifdef UNIT_TEST
int main()
{
    Log &log = Log::instance();
    XMLOutput xmlOutput(new COutPrimitive);
    log.add();
    return 0;
}
#endif
