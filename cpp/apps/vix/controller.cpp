#include "controller.hpp"

ReturnCode Controller::addEvent(Event &event)
{
    MSS_BEGIN(ReturnCode);
    events_.push_back(event);
    MSS_END();
}

ReturnCode Controller::process()
{
    MSS_BEGIN(ReturnCode);
    MSS_Q(!events_.empty(), NoMoreEvents);
    Event event = events_.front();
    events_.pop_front();
    switch (event.type)
    {
        case Event::KeyPressed: MSS(model_.add(event.key.code)); break;
    }
    MSS_END();
}
