#include "log_xml.hpp"
#include "log_primitives.hpp"

using namespace gubg;

unsigned int XMLOutput::_indentLevel = 0;

XMLOutput::XMLOutput(Log::Output::Primitive *primitive):
    Log::Output(primitive)
{
}

void XMLOutput::newLevel(const std::string &fileName, unsigned int lineNr, const std::string &tag)
{
    indent();
    write("<" + tag + ">" + "\n");
    ++_indentLevel;
}

void XMLOutput::closeLevel(const std::string &tag)
{
    --_indentLevel;
    indent();
    write("</" + tag + ">" + "\n");
}

void XMLOutput::newLine(const std::string &fileName, unsigned int lineNr)
{
    indent();
    write("<Line fileName = \"" + fileName + "\", lineNr = \"" + fileName + "\">");
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
