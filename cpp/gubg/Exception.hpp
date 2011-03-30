#ifndef gubg_Exception_hpp
#define gubg_Exception_hpp

#include <stdexcept>
#include <ostream>

namespace gubg
{
    struct Exception: std::runtime_error
    {
        Exception(const std::string &msg):
            std::runtime_error(msg){}
        virtual ~Exception() throw() {}

        template <typename ExceptionT>
        static void raise(ExceptionT &&exc)
        {
            throw ExceptionT(std::move(exc));
        }
    };
}
std::ostream &operator<<(std::ostream &os, const gubg::Exception &exc);

#endif
