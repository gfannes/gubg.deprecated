#ifndef HEADER_rinle_presenter_Presenter_hpp_ALREADY_INCLUDED
#define HEADER_rinle_presenter_Presenter_hpp_ALREADY_INCLUDED

#include "rinle/model/Model.hpp"
#include "rinle/view/Window.hpp"
#include "rinle/presenter/Commander.hpp"

#define GUBG_MODULE_ "Presenter"
#include "gubg/log/begin.hpp"
namespace rinle { namespace presenter {

    class Presenter
    {
        public:
            Presenter(model::Model &model, view::Window &wnd):
                model_(model), wnd_(wnd),
                commander_(*this)
        {
            wnd_.signal.connect(commander_.slot);
            model_.connectLog(wnd_.scroller.slot);
            logger.connect(wnd_.scroller.slot);
        }

            void setCurrent(File file)
            {
                model_.load(file);
                model_.connectCurrent(wnd_.page.slot);
            }

            void proceed_(int nrSteps)
            {
                S();
                {
                    std::ostringstream oss; oss << "Proceeding " << nrSteps;
                    logger.emit(oss.str());
                }
                model_.proceed(nrSteps);
                /*
                auto info = model_.getCurrent();
                if (!info)
                    return;
                wnd_.show(info->path(), info->selectionStart());
                */
            }

        private:
            gubg::pattern::Signal<std::string> logger;
            model::Model &model_;
            view::Window &wnd_;
            Commander<Presenter> commander_;
    };

} }
#include "gubg/log/end.hpp"

#endif
