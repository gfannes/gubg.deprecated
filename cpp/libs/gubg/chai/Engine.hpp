#ifndef HEADER_gubg_chai_Engine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_chai_Engine_hpp_ALREADY_INCLUDED

#include "gubg/Platform.hpp"
#ifdef GUBG_CC_MSC
  //Needed for cl, else, things crash during ctor of ChaiScript instance
  #define CHAISCRIPT_NO_THREADS
#endif
#include "chaiscript/chaiscript.hpp"
#include <memory>

namespace gubg { namespace chai { 

    typedef std::shared_ptr<chaiscript::ChaiScript> Engine_ptr;

    Engine_ptr create_engine();

} } 

#endif
