#include "controller/StateMachine.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
using namespace std;

namespace vix
{
    //MetaState
    void MetaState::changeState(Control control)
    {
        switch (control)
        {
            case Control::Filter: state = filter; break;
            case Control::Content: state = content; break;
            case Control::Command: state = command; break;
        }
    }

    //EditableString
    bool EditableString::processEvent(char ch, MetaState &ms)
    {
        return dispatchEvent(ch);
    }
    bool EditableString::processEvent(Special event, MetaState &ms)
    {
        if (dispatchEvent(event))
            return true;
        return false;
    }
    bool EditableString::dispatchEvent(char ch)
    {
        LOG_SM(EditableString::dispatchEvent, "char " << ch);
        state.append(1, ch);
        signalStateUpdate_();
        return true;
    }
    bool EditableString::dispatchEvent(Special event)
    {
        if (Backspace != event)
            return false;
        if (!state.empty())
        {
            state.resize(state.size()-1);
            signalStateUpdate_();
        }
        return true;
    }
    void EditableString::connect(const Slot &subscriber)
    {
        signal_.connect(subscriber);
    }
    void EditableString::signalStateUpdate_() const
    {
        signal_(&state);
    }
}
