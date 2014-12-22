#ifndef HEADER_imui_State_hpp_ALREADY_INCLUDED
#define HEADER_imui_State_hpp_ALREADY_INCLUDED

#include "imui/Types.hpp"
#include "imui/WidgetId.hpp"
#include "SFML/Graphics.hpp"
#include <vector>

namespace imui { 

    class State
    {
        public:
            backend::KeyEvents keys;
            backend::TextEvents texts;

            struct Mouse
            {
                bool left_button = false;
                bool right_button = false;
                bool left_clicked = false;
                bool right_clicked = false;
                Vector2 position;
            };
            Mouse mouse;

            State(){}

            void process(const sf::RenderWindow &);

            void setHot(const WidgetId &id) const;
            bool isHot(const WidgetId &id) const;

        private:
            //No copying
            State(const State &);
            State &operator=(const State &);

            //We collect _all_ hot ids for one render operation; if widgets overlap, we will
            //take the last drawn (ie, hot_ids_.back())
            typedef std::vector<WidgetId> WidgetIds;
            mutable WidgetIds hot_ids_;
            WidgetId hot_id_;
    };

} 

#endif
