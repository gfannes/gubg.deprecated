#ifndef HEADER_gubg_chai_Engine_hpp_ALREADY_INCLUDED
#define HEADER_gubg_chai_Engine_hpp_ALREADY_INCLUDED

//Needed for cl, else, things crash during ctor of ChaiScript instance
#define CHAISCRIPT_NO_THREADS
#include "chaiscript/chaiscript.hpp"
#include <memory>

namespace gubg { namespace chai { 

    typedef std::shared_ptr<chaiscript::ChaiScript> Engine_ptr;

    Engine_ptr create_engine();

} } 

#endif
