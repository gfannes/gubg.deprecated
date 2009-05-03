#include "log_xml.hpp"
#include "log_primitives.hpp"

#ifdef UNIT_TEST
int main()
{
    Log &log = Log::instance();
    XMLOutput xmlOutput(new COutPrimitive);
    log.add();
    return 0;
}
#endif
