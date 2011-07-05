#ifndef vix_controller_Commander_hpp
#define vix_controller_Commander_hpp

#include "Singleton.hpp"

namespace vix
{
    class Commander
    {
        private:
            friend class gubg::Singleton<Commander>;
            Commander();
            Commander &Commander(const Commander &);

        public:
            add(ICommand)
    };
}

#endif
