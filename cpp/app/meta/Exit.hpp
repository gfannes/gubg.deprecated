#ifndef meta_Exit_hpp
#define meta_Exit_hpp

#include <string>

namespace meta
{
    enum class Status {Ok, Error};
    //Exits the application, printing msg first
    void finalize(const std::string &msg, Status = Status::Error);
}

#endif
