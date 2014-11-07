#ifndef HEADER_fff_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_fff_Compiler_hpp_ALREADY_INCLUDED

#include <memory>

namespace fff { 

    namespace compiler { 
        enum class Type
        {
            GCC, MSC, CLang,
        };
        class Interface
        {
            public:
                virtual ~Interface(){}
        };
    } 

    class Compiler
    {
        public:
            Compiler(compiler::Type);

        private:
            typedef std::shared_ptr<compiler::Interface> Pimpl;
            Pimpl pimpl_;
    };
} 

#endif
