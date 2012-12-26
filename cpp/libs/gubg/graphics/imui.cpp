#include "gubg/graphics/imui.hpp"
#include "gubg/timer.hpp"
#include "gubg/sleep.hpp"
#include "gubg/bitmagic.hpp"
#include "gubg/l.hpp"
using namespace gubg;

#define nullptr ((void*)0)

bool IMUI::processInput()
{
    if (processInput_())
        somethingChanged_ = true;
    return somethingChanged_;
}

void IMUI::reset()
{
    somethingChanged_ = false;
}

bool IMUI::waitForInput(double timeout)
{
    Timer timer(ResetType::NoAuto);
    while (timer.difference() < timeout)
    {
        if (processInput())
            return true;
        //10ms resolution should be fast enough for most things
        nanosleep(0, 10000000);
    }
    return false;
}

Key IMUI::getLastKey()
{
    Key key = lastKey_;
    lastKey_ = Key::Nil;
    return key;
}

bool IMUI::getDigit(unsigned char &digit)
{
    if (convertToDigit(digit, lastKey_))
    {
        getLastKey();
        return true;
    }
    return false;
}

bool IMUI::escapeIsPressed()
{
    if (Key::Escape == lastKey_)
    {
        getLastKey();
        return true;
    }
    return false;
}

bool IMUI::isMouseInside(const TwoPoint<> &region) const
{
    return region.isInside(mousePosition_);
}
bool IMUI::checkMouseButton(MouseButton button, ButtonState cmpState)
{
    bool isUp = false, changed = false;
    switch (button)
    {
        case MouseButton::Left: leftMouse_.get(isUp, changed); break;
        case MouseButton::Middle: middleMouse_.get(isUp, changed); break;
        case MouseButton::Right: rightMouse_.get(isUp, changed); break;
        case MouseButton::WheelUp: wheelUp_.get(isUp, changed); break;
        case MouseButton::WheelDown: wheelDown_.get(isUp, changed); break;
    }
    switch (cmpState)
    {
        case ButtonState::IsDown: return false == isUp; break;
        case ButtonState::IsUp: return true == isUp; break;
        case ButtonState::IsOrWentDown: return false == isUp || changed; break;
        case ButtonState::IsOrWentUp: return true == isUp || changed; break;
    }
    //Should never be reached
    assert(false);
    return false;
}

WidgetProxy::WidgetProxy()
{
    L("ctor: " << std::hex << this);
}
WidgetProxy::WidgetProxy(const WidgetProxy &rhs):
    widget_(rhs.widget_)
{
    L("copy ctor: " << std::hex << this);
}
WidgetProxy::~WidgetProxy()
{
    L("dtor: " << std::hex << this);
}
WidgetProxy &WidgetProxy::operator=(const WidgetProxy &rhs)
{
    L("operator=");
    widget_ = rhs.widget_;
    return *this;
}

void WidgetProxy::set(std::auto_ptr<IWidget> widget)
{
    //Internally, we keep the object in a shared_ptr since WidgetProxy has to be storable in an STL container
    L("widget: " << widget.get());
    widget_.reset(widget.release());
}

WidgetState WidgetProxy::process()
{
    if (!widget_)
        return WidgetState::Empty;
    return widget_->process();
}

namespace
{
    //Instead of assembler hacking, we use this struct to find out who actually called us
    //Place this as the first stack-allocated object in the function that should be tracked
    //Specify how large the arguments and return value of the function are (in bytes) and you can get()
    //a void * to who called us
    struct CallerAddress
    {
        CallerAddress(size_t argSize, size_t returnSize):
            size_(argSize+returnSize){}
        void *get() const
        {
            return get_(sizeof(*this)+size_+sizeof(void*));
        }
        size_t size_;
        private:
        void *get_(size_t offset) const {return (void*)*(unsigned int*)(((char *)this) + offset);}
    };
}
WidgetProxy &Widgets::get(const Location &location, unsigned int extra)
{
#if 0
    //This procedure is not very OK, it gets optimized-out...

    //This object should remain the first one in this function and its constructor
    //arguments should reflect the size of the arguments and return value of this function
    CallerAddress callerAddress(sizeof(extra), sizeof(WidgetProxy *));
    unsigned int id = createId_(callerAddress.get(), extra);
#else
    unsigned int id = *reinterpret_cast<const unsigned int*>(&location.file) + (location.nr << 5) ^ bitmagic::reverseBits(extra);
#endif
    return widgetPerId_[id];
}
unsigned int Widgets::createId_(void *location, unsigned int extra)
{
    return *reinterpret_cast<unsigned int *>(&location) ^ bitmagic::reverseBits(extra);
}
