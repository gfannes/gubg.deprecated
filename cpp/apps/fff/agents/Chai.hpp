#ifndef HEADER_fff_agents_Chai_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Chai_hpp_ALREADY_INCLUDED

#include "fff/Agent.hpp"
#include "gubg/file/File.hpp"
#include "gubg/chai/Engine.hpp"

namespace fff { namespace agents { 

    class Chai: public Agent_itf
    {
        public:
            Chai(const gubg::file::File &script);

			std::string name() const override;
			ReturnCode process(Board &) override;

        private:
            std::string name_;
            gubg::chai::Engine_ptr engine_;
    };

} } 

#endif
