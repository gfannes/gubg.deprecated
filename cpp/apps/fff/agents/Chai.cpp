#include "fff/agents/Chai.hpp"

#define GUBG_MODULE_ "Chai"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 

    Chai::Chai(const gubg::file::File &script): engine_(gubg::chai::create_engine())
    {
    }

    std::string Chai::name() const
    {
        return name_;
    }

    ReturnCode Chai::process(Board &board)
    {
        MSS_BEGIN(ReturnCode);
        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
