#ifndef HEADER_fff_agents_Linker_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Linker_hpp_ALREADY_INCLUDED

#include "gubg/bbs/Agent.hpp"
#include "gubg/OnlyOnce.hpp"

namespace fff { namespace agents { 
    class Linker: public gubg::bbs::Agent_itf
    {
        public:
            std::string name() const override {return "Linker";}
            gubg::bbs::ReturnCode process(gubg::bbs::Board &) override;

        private:
            gubg::OnlyOnce addHashTags_;
            gubg::OnlyOnce run_;
    };
} } 

#endif
