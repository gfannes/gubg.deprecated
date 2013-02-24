#ifndef HEADER_iup_Box_hpp_ALREADY_INCLUDED
#define HEADER_iup_Box_hpp_ALREADY_INCLUDED

#include "iup/Widget.hpp"
#include <vector>

namespace iup
{
    class VBox: public Widget
    {
        public:
            virtual Ihandle *create_()
            {
                L("Box create_");
                std::vector<Ihandle*> handles;
                handles.reserve(widgets_.size()+1);
                for (auto w: widgets_)
                    handles.push_back(*w);
                handles.push_back(0);
                return IupVboxv(handles.data());
            }
        private:
    };
}

#endif
