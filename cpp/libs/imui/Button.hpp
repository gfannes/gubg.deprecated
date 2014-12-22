#ifndef HEADER_imui_Button_hpp_ALREADY_INCLUDED
#define HEADER_imui_Button_hpp_ALREADY_INCLUDED

#include "imui/Event.hpp"
#include "imui/Types.hpp"
#include "imui/Region.hpp"

namespace imui { 

    class Button
    {
        public:
            Button(const Region &, const WidgetId &id);

            WidgetId id() const {return id_;}

            Button &setPosition(const Vector2 &pos);
            Button &setSize(const Vector2 &size);

            Event operator()();

        private:
            const Region &region_;
            const WidgetId id_;
            Vector2 pos_;
            Vector2 size_;
    };

} 

#endif
