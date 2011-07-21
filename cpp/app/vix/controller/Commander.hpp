#ifndef vix_controller_Commander_hpp
#define vix_controller_Commander_hpp

#include "controller/Command.hpp"
#include "Singleton.hpp"

namespace vix
{
    class Commander
    {
        GUBG_SINGLETON(Commander);

        public:
        void add(ICommand::Ptr);
    };
}

#endif
