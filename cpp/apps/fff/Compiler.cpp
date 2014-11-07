#include "fff/Compiler.hpp"
#include "fff/compiler/GCC.hpp"

namespace fff { 

    Compiler::Compiler(compiler::Type type)
    {
        switch (type)
        {
            case compiler::Type::GCC: pimpl_.reset(new compiler::GCC); break;
        }
    }

} 
