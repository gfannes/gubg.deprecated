#ifndef HEADER_gubg_Exception_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Exception_hpp_ALREADY_INCLUDED

#include <stdexcept>
#include <ostream>
#include <iostream>

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
//            std::cout << "THROWING AN EXCEPTION: " << exc << std::endl;
            throw ExceptionT(std::move(exc));
        }
    };
    struct NotImplemented: Exception
    {
        NotImplemented(const std::string &msg):
            Exception(msg){}
    };
}
std::ostream &operator<<(std::ostream &os, const gubg::Exception &exc);

#endif
