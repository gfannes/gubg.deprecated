#include "fff/agents/Chai.hpp"
#include "fff/Board.hpp"
#include <iostream>

#define GUBG_MODULE_ "Chai"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 

    Chai::Chai(const gubg::file::File &script): name_(script.name()), engine_(gubg::chai::create_engine())
    {
        SS("Loading chai script from " << script);

        engine_->add(chaiscript::fun(&Chai::add_callback_, std::ref(*this)), "on_item");

        try
        {
            engine_->eval_file(script.name());
            L("Script was loaded ok!");
        }
        catch (chaiscript::exception::eval_error &err)
        {
            std::cout << "Could not load chai script from \"" << script << "\":" << std::endl << err.pretty_print() << std::endl;
            L("Error loading script: " << err.pretty_print());
            engine_.reset();
        }
    }

    void Chai::add_callback_(const Callback &cb)
    {
        S();L("Adding a callback");
        callback_ = cb;
        cb_was_set_ = true;
        L(STREAM(cb_was_set_));
    }

    std::string Chai::name() const
    {
        return name_;
    }

    ReturnCode Chai::process(Board &board)
    {
        MSS_BEGIN(ReturnCode);

        auto tvs = board.getFrom(ix_);
        L(STREAM(cb_was_set_, ix_, tvs.size()));
        ix_ += tvs.size();

        auto nr_left = tvs.size();
        for (auto tv: tvs)
        {
            if (cb_was_set_)
            {
                L("Calling into the callback");
                callback_(tv.tag.to_str(), tv.value.as_string(), --nr_left);
            }
        }

        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
