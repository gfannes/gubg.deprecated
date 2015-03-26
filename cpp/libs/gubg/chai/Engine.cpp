#include "gubg/chai/Engine.hpp"
#include "chaiscript/chaiscript_stdlib.hpp"

#define GUBG_MODULE_ "chai"
#include "gubg/log/begin.hpp"
namespace gubg { namespace chai { 

    Engine_ptr create_engine()
    {
        S();
        Engine_ptr eng;
        try
        {
            eng.reset(new chaiscript::ChaiScript(chaiscript::Std_Lib::library()));
        }
        catch (...)
        {
            L("Failed to create the engine");
        }
        return eng;
    }

} } 
#include "gubg/log/end.hpp"
