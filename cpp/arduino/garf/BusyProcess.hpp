#ifndef garf_BusyProcess_hpp
#define garf_BusyProcess_hpp

namespace garf
{
    template <long Duration, typename Object>
        void busyProcess(Object &object)
        {
            for (long i = 0; i < Duration; ++i)
            {
                delay(1);
                object.process(1);
            }
        }
}

#endif
