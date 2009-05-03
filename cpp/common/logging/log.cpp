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
    _outputs.push_back(output);
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

Log::Output::Primitive *Log::Output::swapPrimitive(Log::Output::Primitive *primitive)
{
    Log::Output::Primitive *origPrimitive = _primitive;
    _primitive = primitive;
    return origPrimitive;
}


#ifdef UNIT_TEST
int main()
{
    Log &log = Log::instance();
    return 0;
}
#endif
