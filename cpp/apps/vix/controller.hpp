#ifndef HEADER_vix_controller_hpp_ALREADY_INCLUDED
#define HEADER_vix_controller_hpp_ALREADY_INCLUDED

#include "model.hpp"
#include "view.hpp"
#include "codes.hpp"
#include "types.hpp"
#include <deque>

class Controller
{
    public:
        Controller(Model &model, View &view):
            model_(model), view_(view){}

        ReturnCode addEvent(Event &);

        //Processes queued events and notifies the model/view if necessary
        ReturnCode process();

    private:
        Model &model_;
        View &view_;

        typedef std::deque<Event> Events;
        Events events_;
};

#endif
