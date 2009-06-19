#include "log.hpp"

#include "log_primitives.hpp"
#include "log_xml.hpp"
#include "log_tree.hpp"

#include <sstream>

using namespace gubg;

Log::Log()
{
}

Log::~Log()
{
    for (std::vector<Output*>::iterator it = _outputs.begin(); it != _outputs.end(); ++it)
	delete *it;
}

Log &Log::instance()
{
    static Log log;
    return log;
}

void Log::add(Log::LogFormat logFormat, Log::LogSink logSink, const std::string &fileName)
{
    Output::Primitive *primitive = 0;
    switch (logSink)
    {
    case COut:
	primitive = new COutPrimitive();
	break;
    case File:
	primitive = new FilePrimitive(fileName);
	break;
    default:
	throw new std::exception();
	break;
    }

    Output *output = 0;
    switch (logFormat)
    {
    case XML:
	output = new XMLOutput(primitive);
	break;
    case Tree:
	output = new TreeOutput(primitive);
	break;
    default:
	throw new std::exception();
	break;
    }

    Log &log = Log::instance();
    log._outputs.push_back(output);
}

void Log::newLevel(const std::string &fileName, unsigned int lineNr, const std::string &tag)
{
    std::vector<Output *>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); ++it)
    {
	(*it)->newLevel(fileName, lineNr, tag);
    }
}

void Log::closeLevel(const std::string &tag)
{
    std::vector<Output *>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); ++it)
    {
	(*it)->closeLevel(tag);
    }
}

void Log::newLine(const std::string &fileName, unsigned int lineNr)
{
    std::vector<Output *>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); ++it)
    {
	(*it)->newLine(fileName, lineNr);
    }
}

void Log::closeLine()
{
    std::vector<Output *>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); ++it)
    {
	(*it)->closeLine();
    }
}

Log &Log::operator<<(const std::string &str)
{
    std::vector<Output *>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); ++it)
    {
	(*it)->operator<<(str);
    }
    return *this;
}

Log::Output::Output()
    :_primitive(0)
{
}

Log::Output::Output(Primitive *primitive):
    _primitive(primitive)
{
}

Log::Output::~Output()
{
    if (_primitive)
	delete _primitive;
}

void Log::Output::write(const std::string &str)
{
    _primitive->write(str);
}

Log::Output::Primitive *Log::Output::swapPrimitive(Log::Output::Primitive *primitive)
{
    Log::Output::Primitive *origPrimitive = _primitive;
    _primitive = primitive;
    return origPrimitive;
}

Log::Scope::Scope(const std::string &fileName, unsigned int lineNr, const std::string &msg):
    _msg(msg),
    _lineIsOpen(false)
{
    Log &log = Log::instance();
    log.newLevel(fileName, lineNr, _msg);
}

Log::Scope::~Scope()
{
    Log &log = Log::instance();
    if (_lineIsOpen)
	log.closeLine();
    log.closeLevel(_msg);
}

Log::Scope &Log::Scope::operator()(const std::string &fileName, unsigned int lineNr, const std::string &msg)
{
    Log &log = Log::instance();
    if (_lineIsOpen)
	log.closeLine();
    log.newLine(fileName, lineNr);
    _lineIsOpen = true;
    log << msg;
    return *this;
}

Log::Scope &Log::Scope::operator<<(const std::string &msg)
{
    Log &log = Log::instance();
    if (!_lineIsOpen)
	throw new std::exception();
    log << " " << msg;
    return *this;
}

Log::Scope &Log::Scope::operator<<(int value)
{
    Log &log = Log::instance();
    if (!_lineIsOpen)
	throw new std::exception();
    std::ostringstream str;
    str << " " << value;
    log << str.str();
    return *this;
}

Log::Scope &Log::Scope::operator<<(unsigned int value)
{
    Log &log = Log::instance();
    if (!_lineIsOpen)
	throw new std::exception();
    std::ostringstream str;
    str << " 0x" << std::hex << value;
    log << str.str();
    return *this;
}

Log::Scope &Log::Scope::operator<<(void *ptr)
{
    Log &log = Log::instance();
    if (!_lineIsOpen)
	throw new std::exception();
    std::ostringstream str;
    str << " ptr 0x" << std::hex << ptr;
    log << str.str();
    return *this;
}

#ifdef UNIT_TEST
#include "log_xml.hpp"
#include "log_primitives.hpp"
int main()
{
    Log::add(Log::XML, Log::COut, "test.log");
    {
	Log::Scope scope(__FILE__, __LINE__, "Scope1");
	{
	    Log::Scope scope(__FILE__, __LINE__, "Scope11");
	    {
		Log::Scope scope(__FILE__, __LINE__, "Scope111");
		scope(__FILE__, __LINE__, "Opening a new line");
		scope << "Line1 at scope 111.";
		scope << "Line2 at scope 111.";
		scope(__FILE__, __LINE__, "New line:") << "Nothing to say.";
	    }
	    {
		Log::Scope scope(__FILE__, __LINE__, "Scope112");
	    }
	    {
		Log::Scope scope(__FILE__, __LINE__, "Scope113");
	    }
	}
	{
	    Log::Scope scope(__FILE__, __LINE__, "Scope12");
	    {
		Log::Scope scope(__FILE__, __LINE__, "Scope121");
	    }
	    {
		Log::Scope scope(__FILE__, __LINE__, "Scope122");
	    }
	    {
		Log::Scope scope(__FILE__, __LINE__, "Scope123");
	    }
	}
    }
    {
	Log::Scope scope(__FILE__, __LINE__, "Scope2");
    }
    return 0;
}
#endif
