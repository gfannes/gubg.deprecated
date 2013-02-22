#ifndef HEADER_iup_Dialog_hpp_ALREADY_INCLUDED
#define HEADER_iup_Dialog_hpp_ALREADY_INCLUDED

#include "iup/Widget.hpp"
#include "gubg/crtp.hpp"
#include <string>
#include "gubg/l.hpp"

namespace iup
{
    template <typename ChildWidget>
        class Dialog: public Widget
        {
            public:
                Dialog()
                {
                    addChild(childWidget_);
                }
                ~Dialog()
                {
                    if (handle_)
                        IupDestroy(handle_);
                }

                template <typename String>
                    Dialog &setTitle(const String &title) { setAttribute("TITLE", title); return *this; }

                void run()
                {
                    if (!handle_)
                        create();
                    IupMap(handle_);
                    mapped();
                    IupShow(handle_);
                    IupMainLoop();
                }

            protected:
                virtual Ihandle *create_() { return IupDialog(childWidget_); }

            private:
                ChildWidget childWidget_;
        };
}

#endif
