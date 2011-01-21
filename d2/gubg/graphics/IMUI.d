module gubg.graphics.IMUI;

import gubg.graphics.Canvas;
import gubg.Point;
import gubg.BitMagic;
import gubg.StateMachine;
import gubg.Timer;
import gubg.Math;
import derelict.sdl.sdl;
public import std.range;
import core.thread;

import std.stdio;

enum WidgetState {Empty, Emerging, Idle, Highlighted, Selected, Activating, Activated};
interface IWidget
{
    //Processes the widget (draw etc. if any is present)
    //and returns its current state
    WidgetState process();
}
//We don't actually listen to any event, we just process
enum Alignment {Left, Center};
class Button: StateMachine!(bool, WidgetState),  IWidget
{
    this (TwoPoint dimensions, string label, Alignment alignment, SDLCanvas canvas)
    {
        dimensions_ = dimensions;
        label_ = label;
        alignment_ = alignment;
        canvas_ = canvas;
        super(WidgetState.Emerging);
    }
    Button setLabel(string label)
    {
        label_ = label;
        return this;
    }
    //StateMachine interface
    bool processEvent(bool)
    {
        switch (state)
        {
            case WidgetState.Emerging: changeState(WidgetState.Idle); break;
            case WidgetState.Idle:
                                       if (canvas_.imui.isMouseInside(dimensions_) && canvas_.imui.isMouseButton(MouseButton.Left, ButtonState.Up))
                                           changeState(WidgetState.Highlighted);
                                       break;
            case WidgetState.Highlighted:
                                       if (canvas_.imui.isMouseInside(dimensions_))
                                       {
                                           if (canvas_.imui.isMouseButton(MouseButton.Left, ButtonState.Down))
                                               changeState(WidgetState.Activating);
                                       }
                                       else
                                           changeState(WidgetState.Idle);
                                       break;
            case WidgetState.Activating:
                                       if (canvas_.imui.isMouseButton(MouseButton.Left, ButtonState.Up))
                                           changeState(WidgetState.Activated);
                                       break;
            case WidgetState.Activated:
                                       changeState(WidgetState.Idle);
                                       break;
        }
        return true;
    }
    //IWidget interface
    WidgetState process()
    {
        processEvent(false);
        Style s;
        switch (state)
        {
            case WidgetState.Idle: break;
            case WidgetState.Highlighted: s.fill(Color.coolGreen); break;
            case WidgetState.Activating: s.fill(Color.yellow); break;
            case WidgetState.Activated: s.fill(Color.green); break;
        }
        canvas_.drawRectangle(dimensions_, s);
        //Draw the label
        if (!label_.empty)
        {
            Style ts;
            ts.fill(Color.black).stroke(Color.white).width(2.0);
            HAlign ha;
            switch (alignment_)
            {
                case Alignment.Center: ha = HAlign.Center; break;
                case Alignment.Left: ha = HAlign.Left; break;
            }
            auto lh = dimensions_.height*0.75;
            auto lw = dimensions_.width - (dimensions_.height-lh);
            canvas_.drawText(label_, TwoPoint.centered(dimensions_.centerX, dimensions_.centerY, lw, lh), VAlign.Center, ha, ts);
        }
        return state;
    }

    private:
    TwoPoint dimensions_;
    string label_;
    Alignment alignment_;
    SDLCanvas canvas_;
}
class Scroller: StateMachine!(bool, WidgetState),  IWidget
{
    this (TwoPoint displayArea, TwoPoint listenArea, SDLCanvas canvas)
    {
        displayArea_ = displayArea;
        listenArea_ = listenArea;
        setRange([0.0, 1.0]);
        setBar([0.3, 0.5]);
        canvas_ = canvas;
        super(WidgetState.Emerging);
    }
    Scroller setRange(real[2] range)
    {
        range_[] = range[];
        //Compute the linear transformation that transforms range_ into displayArea_.p[10].y
        computeLinTrans!(real, real, real)(linTrans_, range_, [displayArea_.p1.y, displayArea_.p0.y]);
        return this;
    }
    Scroller setRange(uint nr)
    {
        return setRange([0.0, nr]);
    }
    Scroller setBar(real[2] bar)
    {
        bar_[] = bar;
        return this;
    }
    Scroller setBar(uint ix)
    {
        return setBar([cast(real)ix, ix+1]);
    }
    //StateMachine interface
    bool processEvent(bool)
    {
        return false;
    }
    //IWidget interface
    WidgetState process()
    {
        processEvent(false);
        Style s;
        s.fill(Color.purple);
//        switch (state)
//        {
//            case WidgetState.Idle: break;
//            case WidgetState.Highlighted: s.fill(Color.coolGreen); break;
//            case WidgetState.Activating: s.fill(Color.yellow); break;
//            case WidgetState.Activated: s.fill(Color.green); break;
//        }
        
        canvas_.drawRectangle(TwoPoint(displayArea_.p0.x, transformLinTrans(bar_[0], linTrans_), displayArea_.p1.x, transformLinTrans(bar_[1], linTrans_)), s);
        return state;
    }

    private:
    TwoPoint displayArea_;
    TwoPoint listenArea_;
    real[2] linTrans_;
    real[2] range_;
    real[2] bar_;
    SDLCanvas canvas_;
}

class Widgets
{
    WidgetProxy get(uint extra = 0)
    {
        void *callerLocation;
        asm
        {
            mov EAX, dword ptr [EBP+4];
            mov callerLocation[EBP], EAX;
        }
        uint id = createId_(callerLocation, extra);
        WidgetProxy *wp = (id in widgetPerId);
        if (!wp)
        {
            auto w = new WidgetProxy(id);
            widgetPerId[id] = w;
            wp = &w;
        }
        return *wp;
    }
    class WidgetProxy: IWidget
    {
        this(uint id)
        {}
        void set(IWidget widget){widget_ = widget;}
        T get(T)(){return cast(T)widget_;}
        //IWidget interface
        WidgetState process()
        {
            if (widget_ is null)
                return WidgetState.Empty;
            return widget_.process();
        }
        private:
        IWidget widget_;
    }
    private:
    uint createId_(void *location, uint extra)
    {
        return cast(uint)location ^ gubg.BitMagic.reverseBits(extra);
    }
    WidgetProxy[uint] widgetPerId;
}

//Key is basically the same order as SDL uses...
Key fromSDL(int sdlKey){return cast(Key)sdlKey;}
bool convertToDigit(out ubyte digit, in Key key)
{
    if (Key.Dec0 <= key && key <= Key.Dec9)
    {
        digit = cast(ubyte)(key - Key.Dec0);
        return true;
    }
    if (Key.DecKP0 <= key && key <= Key.DecKP9)
    {
        digit = cast(ubyte)(key - Key.DecKP0);
        return true;
    }
    return false;
}
char convertToChar(in Key key)
{
    ubyte digit;
    if (convertToDigit(digit, key))
        return cast(char)('0'+digit);
    if (Key.a <= key && key <= Key.z)
        return cast(char)('a'+key-Key.a);
    return '.';
}
enum Key
{
    None = 0,

    //Normal numbers
    Dec0 = SDLK_0,
    Dec1 = SDLK_1,
    Dec2 = SDLK_2,
    Dec3 = SDLK_3,
    Dec4 = SDLK_4,
    Dec5 = SDLK_5,
    Dec6 = SDLK_6,
    Dec7 = SDLK_7,
    Dec8 = SDLK_8,
    Dec9 = SDLK_9,
    //Keypad numbers
    DecKP0 = SDLK_KP0,
    DecKP1 = SDLK_KP1,
    DecKP2 = SDLK_KP2,
    DecKP3 = SDLK_KP3,
    DecKP4 = SDLK_KP4,
    DecKP5 = SDLK_KP5,
    DecKP6 = SDLK_KP6,
    DecKP7 = SDLK_KP7,
    DecKP8 = SDLK_KP8,
    DecKP9 = SDLK_KP9,

    //Function keys
    F1 = SDLK_F1,
    F2 = SDLK_F2,
    F3 = SDLK_F3,
    F4 = SDLK_F4,
    F5 = SDLK_F5,
    F6 = SDLK_F6,
    F7 = SDLK_F7,
    F8 = SDLK_F8,
    F9 = SDLK_F9,
    F10 = SDLK_F10,
    F11 = SDLK_F11,
    F12 = SDLK_F12,
    F13 = SDLK_F13,
    F14 = SDLK_F14,
    F15 = SDLK_F15,

    a = SDLK_a,
    b = SDLK_b,
    c = SDLK_c,
    d = SDLK_d,
    e = SDLK_e,
    f = SDLK_f,
    g = SDLK_g,
    h = SDLK_h,
    i = SDLK_i,
    j = SDLK_j,
    k = SDLK_k,
    l = SDLK_l,
    m = SDLK_m,
    n = SDLK_n,
    o = SDLK_o,
    p = SDLK_p,
    q = SDLK_q,
    r = SDLK_r,
    s = SDLK_s,
    t = SDLK_t,
    u = SDLK_u,
    v = SDLK_v,
    w = SDLK_w,
    x = SDLK_x,
    y = SDLK_y,
    z = SDLK_z,

    Escape = SDLK_ESCAPE,
}
enum MouseButton
{
    Left,
    Middle,
    Right,
}
enum ButtonState
{
    Up,
    Down,
}

//Immediate-mode user interface
abstract class IMUI
{
    //Call this fast enough to get a reasonable response time
    //Returns true if some event is waiting (not sure if this maps OK for all input devices)
    //Calling this twice without getting any event should return the same
    bool processInput();
    //Returns true as soon as input is ready to be processed
    //false if it timed out
    bool waitForInput(real timeout)
    {
        auto timer = Timer(ResetType.NoAuto);
        while (timer.difference < timeout)
        {
            if (processInput())
                return true;
            //10ms resolution should be fast enough for most things
            Thread.sleep(10_0000);
        }
        return false;
    }

    //This is a non-const method, it will reset the interal lastKey_
    Key getLastKey()
    {
        Key key = lastKey_;
        lastKey_ = Key.None;
        return key;
    }
    //This is a non-const method, it will reset the internal lastKey_ if a digit was found
    bool getDigit(out ubyte digit)
    {
        if (convertToDigit(digit, lastKey_))
        {
            getLastKey();
            return true;
        }
        return false;
    }
    //This is a non-const method, it will reset the internal lastKey_ if ESC was found
    bool escapeIsPressed()
    {
        if (Key.Escape == lastKey_)
        {
            getLastKey();
            return true;
        }
        return false;
    }

    bool isMouseInside(TwoPoint region) const
    {
        return region.isInside(mousePosition_);
    }
    bool isMouseButton(MouseButton button, ButtonState cmpState)
    {
        bool state;
        switch (button)
        {
            case MouseButton.Left: state = leftMouse_; break;
            case MouseButton.Middle: state = middleMouse_; break;
            case MouseButton.Right: state = rightMouse_; break;
        }
        switch (cmpState)
        {
            case ButtonState.Down: return true == state; break;
            case ButtonState.Up: return false == state; break;
        }
        assert(false);
        return false;
    }

    protected:
    Key lastKey_ = Key.None;
    //When processInput() polls for key presses, it can store excess Keys in here and move them
    //later to lastKey_ one by one
    Key[] cachedKeys_;
    Point mousePosition_ = Point(0.0, 0.0);
    //A value of true means it is down
    bool leftMouse_;
    bool middleMouse_;
    bool rightMouse_;
}

version (UnitTest)
{
    import core.thread;
    void main()
    {
        auto canvas = new SDLCanvas(640, 480);
        int i = 0;
        while (!canvas.imui.escapeIsPressed)
        {
            canvas.imui.processInput();
            ++i;
            if (i > 100)
                return;
            //Sleep for 10ms
            Thread.sleep(100000);
        }
    }
}
