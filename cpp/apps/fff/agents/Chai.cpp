#include "fff/agents/Chai.hpp"
#include "gubg/bbs/Board.hpp"
#include <iostream>

#define GUBG_MODULE_ "Chai"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 

    Chai::Chai(const gubg::file::File &script): engine_(gubg::chai::create_engine())
    {
        SS("Loading chai script from " << script);

        engine_->add(chaiscript::fun(&Chai::add_callback_, std::ref(*this)), "on_item");
        engine_->add(chaiscript::fun(&Chai::add_item_, std::ref(*this)), "add_item");

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

    void Chai::add_callback_(const std::string &name, const Callback &cb)
    {
        S();L("Adding callback " << name);
        name_ = name;
        callback_ = cb;
        cb_was_set_ = true;
        L(STREAM(cb_was_set_));
    }
    void Chai::add_item_(const std::string &tag, const std::string &value)
    {
        S();L("Adding item " << STREAM(tag, value));
        items_.push(std::make_pair(tag, value));
    }

    std::string Chai::name() const
    {
        return name_;
    }

    gubg::bbs::ReturnCode Chai::process(gubg::bbs::Board &board)
    {
        MSS_BEGIN(gubg::bbs::ReturnCode);

        auto tvs = board.getFrom(ix_);
        L(STREAM(cb_was_set_, ix_, tvs.size()));
        ix_ += tvs.size();

        auto nr_left = tvs.size();
        for (auto tv: tvs)
        {
            if (cb_was_set_)
            {
                L("Calling into the callback");
                callback_(tv.tag, tv.value, --nr_left);
            }
        }

        while (!items_.empty())
        {
            auto p = items_.front(); items_.pop();
            board.addItem(p.first, p.second);
        }

        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
