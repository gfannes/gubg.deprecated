#ifndef HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED
#define HEADER_rinle_view_Window_hpp_ALREADY_INCLUDED

#include "rinle/view/Util.hpp"
#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <memory>


namespace rinle
{
    namespace view
    {
        inline void run() {nana::gui::exec();}

        class Window
        {
            public:
                Window():
                    width_(1365), height_(800),
                    wnd_(nana::rectangle(0, 0, width_, height_))
                {
                    wnd_.caption(STR("Rinle"));
                    wnd_.show();

                    const auto labelHeight = height_/NrRows;
                    for (int i = 0; i < NrRows; ++i)
                    {
                        auto label = std::make_shared<nana::gui::label>(wnd_, nana::rectangle(0, i*labelHeight, width_, labelHeight));
                        labels_.push_back(label);
                    }
                }

                template <typename Iterator>
                    void showLines(Iterator begin, Iterator end)
                    {
                        for (auto &label: labels_)
                        {
                            nana::string str;
                            if (begin != end)
                                convert(str, (begin++)->txt);
                            label->caption(str);
                        }
                        wnd_.show();
                    }

                /*
    nana::string content(tmp.size(), '\0');
    label lbl(fm, s);
    lbl.caption(content);
    lbl.make_event<events::click>(API::exit);
    label lbl2(fm, s2);
    lbl2.caption(content);
    fm.show();
    */
            private:
                static const int NrRows = 40;
                const int width_;
                const int height_;
                nana::gui::form wnd_;
                std::vector<std::shared_ptr<nana::gui::label>> labels_;
        };
    }
}

#endif
