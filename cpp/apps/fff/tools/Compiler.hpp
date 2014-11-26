#ifndef HEADER_fff_tools_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_fff_tools_Compiler_hpp_ALREADY_INCLUDED

#include "fff/Tool.hpp"
#include "fff/Compiler.hpp"
#include "gubg/OnlyOnce.hpp"

namespace fff { namespace tools { 
    class Compiler: public Tool_itf
    {
        public:
            Compiler();

            std::string name() const override {return "Compiler";}
            ReturnCode process(Board &) override;

        private:
            size_t ix_ = 0;
            fff::Compiler compiler_;
            gubg::OnlyOnce is_default_compiler_;
            bool build_type_was_set_ = false;
    };
} } 

#endif
