#ifndef HEADER_gubg_condition_variable_hpp_ALREADY_INCLUDED
#define HEADER_gubg_condition_variable_hpp_ALREADY_INCLUDED

#ifdef GUBG_LINUX
#include <condition_variable>
#endif

#ifdef GUBG_MINGW
#include "boost/thread/condition_variable.hpp"
namespace std
{
    typedef ::boost::condition_variable condition_variable;
}
#endif

#endif