#include "log.hpp"

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

void Log::add(Log::Output *output)
{
    Log &log = Log::instance();
    log._outputs.push_back(output);
}

void Log::newLevel(const std::string &tag)
{
    std::vector<Output *>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); ++it)
    {
	(*it)->newLevel(tag);
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

void Log::newLine()
{
    std::vector<Output *>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); ++it)
    {
	(*it)->newLine();
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

Log::Output &Log::operator<<(const std::string &str)
{
    std::vector<Output *>::iterator it;
    for (it = _outputs.begin(); it != _outputs.end(); ++it)
    {
	(*it)->operator<<(str);
    }
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

Log::Scope::Scope(const std::string &msg):
    _msg(msg),
    _lineIsOpen(false)
{
    Log &log = Log::instance();
    log.newLevel(_msg);
}

Log::Scope::~Scope()
{
    Log &log = Log::instance();
    if (_lineIsOpen)
	log.closeLine();
    log.closeLevel(_msg);
}

Log::Scope &Log::Scope::operator<<(const std::string &msg)
{
    Log &log = Log::instance();
    if (!_lineIsOpen)
    {
	log.newLine();
	_lineIsOpen = true;
    } else
	log << " ";
    log << msg;
    return *this;
}

Log::Scope &Log::Scope::operator()(const std::string &msg)
{
    Log &log = Log::instance();
    if (_lineIsOpen)
	log.closeLine();
    log.newLine();
    _lineIsOpen = true;
    log << msg;
    return *this;
}

#ifdef UNIT_TEST
#include "log_xml.hpp"
#include "log_primitives.hpp"
int main()
{
    Log::add(new gubg::XMLOutput(new gubg::COutPrimitive));
    {
	Log::Scope scope("Scope1");
	{
	    Log::Scope scope("Scope11");
	    {
		Log::Scope scope("Scope111");
		scope << "Line1 at scope 111.";
		scope << "Line2 at scope 111.";
		scope("New line:") << "Nothing to say.";
	    }
	    {
		Log::Scope scope("Scope112");
	    }
	    {
		Log::Scope scope("Scope113");
	    }
	}
	{
	    Log::Scope scope("Scope12");
	    {
		Log::Scope scope("Scope121");
	    }
	    {
		Log::Scope scope("Scope122");
	    }
	    {
		Log::Scope scope("Scope123");
	    }
	}
    }
    {
	Log::Scope scope("Scope2");
    }
    return 0;
}
#endif
