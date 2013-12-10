#ifndef HEADER_rinle_presenter_Presenter_hpp_ALREADY_INCLUDED
#define HEADER_rinle_presenter_Presenter_hpp_ALREADY_INCLUDED

#include "rinle/model/Model.hpp"
#include "rinle/view/Window.hpp"
#include "rinle/presenter/Commander.hpp"

#define GUBG_MODULE_ "Presenter"
#include "gubg/log/begin.hpp"
namespace rinle
{
    namespace presenter
    {
        class Presenter
        {
            public:
                Presenter(model::Model &model, view::Window &wnd):
                    model_(model), wnd_(wnd),
                    commander_(*this)
            {
                wnd_.addObserver(commander_.observer);
            }

                void setCurrent(File file)
                {
                    model_.load(file);
                }

				void proceed_(int nrSteps)
				{
                    S();
                    model_.proceed(nrSteps);
                    auto info = model_.getCurrent();
                    if (!info)
                        return;
                    wnd_.show(info->path(), info->selectionStart());
				}

            private:
                model::Model &model_;
                view::Window &wnd_;
                Commander<Presenter> commander_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
