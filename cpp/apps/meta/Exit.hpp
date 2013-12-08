#ifndef HEADER_meta_Exit_hpp_ALREADY_INCLUDED
#define HEADER_meta_Exit_hpp_ALREADY_INCLUDED

#include <string>

namespace meta
{
    enum class Status {Ok, Error};
    //Exits the application, printing msg first
    void finalize(const std::string &msg, Status = Status::Error);
}

#endif
