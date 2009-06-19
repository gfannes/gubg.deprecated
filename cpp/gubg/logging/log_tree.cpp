#include "log_tree.hpp"
#include "log_primitives.hpp"

#include <sstream>

using namespace gubg;

unsigned int TreeOutput::_indentLevel = 0;

TreeOutput::TreeOutput(Log::Output::Primitive *primitive):
    Log::Output(primitive)
{
}

void TreeOutput::newLevel(const std::string &fileName, unsigned int lineNr, const std::string &tag)
{
    indent();
    std::ostringstream str;
    str << fileName << ":" << lineNr << " >>> " << tag << std::endl;
    write(str.str());
    ++_indentLevel;
}

void TreeOutput::closeLevel(const std::string &tag)
{
    --_indentLevel;
    indent();
    write("<<< " + tag + "\n");
}

void TreeOutput::newLine(const std::string &fileName, unsigned int lineNr)
{
    indent();
}

void TreeOutput::closeLine()
{
    write("\n");
}

Log::Output &TreeOutput::operator<<(const std::string &str)
{
    write(str);
}

void TreeOutput::indent()
{
    std::string id(_indentLevel*2, ' ');
    write(id);
}

#ifdef UNIT_TEST
int main()
{
    Log &log = Log::instance();
    TreeOutput treeOutput(new COutPrimitive);
    log.add();
    return 0;
}
#endif
