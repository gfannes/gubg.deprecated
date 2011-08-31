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
            case Control::NameFilter: state = nameFilter; break;
            case Control::ContentFilter: state = contentFilter; break;
            case Control::Command: state = command; break;
        }
    }

    //EditableString
    bool EditableString::processEvent(char ch, MetaState &ms)
    {
        LOG_SM_(Debug, EditableString::processEvent, "char: " << ch);
        return this->dispatchEvent(ch);
    }
    bool EditableString::processEvent(Special event, MetaState &ms)
    {
        if (dispatchEvent(event))
            return true;
        switch (event)
        {
            case Escape:
                if (state.empty())
                {
                    ms.changeState(Control::NameFilter);
                    return true;
                }
                break;
            default: return false; break;
        }
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
        switch (event)
        {
            case Backspace:
                if (!state.empty())
                {
                    state.resize(state.size()-1);
                    signalStateUpdate_();
                    return true;
                }
                break;
            case Escape:
                if (!state.empty())
                {
                    state.clear();
                    signalStateUpdate_();
                    return true;
                }
                break;
            default: return false; break;
        }
        return false;
    }
    void EditableString::connect(const Slot &subscriber)
    {
        signal_.connect(subscriber);
    }
    void EditableString::signalStateUpdate_() const
    {
        signal_(&state);
    }

    //NameFilterStateMachine
    bool NameFilterStateMachine::processEvent(char ch, MetaState &ms)
    {
        LOG_SM_(Debug, NameFilterSM::processEvent, "char: " << ch);
        if (':' == ch)
            ms.changeState(Control::Command);
        else if ('/' == ch)
            ms.changeState(Control::ContentFilter);
        else
            return EditableString::processEvent(ch, ms);
        return true;
    }

    //ContentFilterStateMachine
    bool ContentFilterStateMachine::processEvent(char ch, MetaState &ms)
    {
        LOG_SM_(Debug, ContentFilterSM::processEvent, "char: " << ch);
        if ('/' == ch)
            return true;
        return EditableString::processEvent(ch, ms);
    }

    //CommandStateMachine
    bool CommandStateMachine::processEvent(char ch, MetaState &ms)
    {
        LOG_SM_(Debug, CommandSM::processEvent, "char: " << ch);
        if (':' == ch)
            return true;
        return EditableString::processEvent(ch, ms);
    }
}
