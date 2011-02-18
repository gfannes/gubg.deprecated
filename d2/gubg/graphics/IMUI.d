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

enum WidgetState {Empty, Emerging, Idle, Highlighted, Selected, Activating, Activated, Moving, ScrollDown, ScrollUp};
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
    Button setFillColor(Color fillColor)
    {
        fillColor_ = fillColor;
        return this;
    }
    Button resetFillColor()
    {
        fillColor_ = Color.invalid;
        return this;
    }
    //StateMachine interface
    bool processEvent(bool)
    {
        switch (state)
        {
            case WidgetState.Emerging: changeState(WidgetState.Idle); break;
            case WidgetState.Idle:
                                       if (canvas_.imui.isMouseInside(dimensions_) && canvas_.imui.checkMouseButton(MouseButton.Left, ButtonState.IsOrWentUp))
                                           changeState(WidgetState.Highlighted);
                                       break;
            case WidgetState.Highlighted://Hoovering over the button
                                       if (canvas_.imui.isMouseInside(dimensions_))
                                       {
                                           if (canvas_.imui.checkMouseButton(MouseButton.Left, ButtonState.IsOrWentDown))
                                               changeState(WidgetState.Activating);
                                       }
                                       else
                                           changeState(WidgetState.Idle);
                                       break;
            case WidgetState.Activating://Button-down
                                       if (canvas_.imui.checkMouseButton(MouseButton.Left, ButtonState.IsOrWentUp))
                                           changeState(WidgetState.Activated);
                                       break;
            case WidgetState.Activated://Button went back up
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
            case WidgetState.Idle:
                if (fillColor_.isValid)
                    s.fill(fillColor_);
                break;
            case WidgetState.Highlighted: s.fill(Color.darkBlue); break;
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
    Color fillColor_ = Color.invalid;
    Alignment alignment_;
    SDLCanvas canvas_;
}
class Scroller: StateMachine!(bool, WidgetState),  IWidget
{
    this (TwoPoint displayArea, TwoPoint listenArea, SDLCanvas canvas)
    {
        displayArea_ = displayArea;
        listenArea_ = listenArea;
        //Some default range and bar to get started
        setRange([0.0, 1.0]);
        setBar([0.0, 1.0]);
        canvas_ = canvas;
        super(WidgetState.Emerging);
    }
    Scroller setRange(real[2] range)
    {
        range_[] = range[];
        //Compute the linear transformation that transforms range_ into displayArea_.p[1|0].y
        computeLinTrans!(real, real, real)(linTrans_, range_, [displayArea_.p1.y, displayArea_.p0.y]);
        return this;
    }
    Scroller setRange(uint nr)
    {
        return setRange([0.0, nr]);
    }
    Scroller setBar(real[2] bar)
    {
        barCenter_ = 0.5*(bar[0]+bar[1]);
        barSize_ = bar[1] - bar[0];
        return this;
    }
    Scroller setBar(uint ix)
    {
        return setBar([cast(real)ix, ix+1]);
    }
    void getBar(ref real[2] bar)
    {
        bar[0] = barCenter_ - 0.5*barSize_;
        bar[1] = barCenter_ + 0.5*barSize_;
    }
    T getBarCenter(T)()
        if(is(typeof(T): real))
    {
        return barCenter_;
    }
    T getBarCenter(T)()
        if(is(typeof(T): uint))
    {
        return barCenter_ - 0.5*barSize_;
    }
    //StateMachine interface
    bool processEvent(bool)
    {
        switch (state)
        {
            case WidgetState.Emerging: changeState(WidgetState.Idle); break;
            case WidgetState.Idle:
                                       //Becomes true if we are inside the diplay or the listen area. We don't compute this in advance
                                       //for performance reasons, maybe we don't have to check if we are in the listen area
                                       bool inDisplay = canvas_.imui.isMouseInside(displayArea_);
                                       //We check for the left button to be up, making sure we don't enter WidgetState.Highlighted with
                                       //the left button already down
                                       if (inDisplay && canvas_.imui.checkMouseButton(MouseButton.Left, ButtonState.IsOrWentUp))
                                           changeState(WidgetState.Highlighted);
                                       else
                                       {
                                           //Check for a scroll event in the display or listen area
                                           if (inDisplay || canvas_.imui.isMouseInside(listenArea_))
                                           {
                                               if (canvas_.imui.checkMouseButton(MouseButton.WheelDown, ButtonState.IsOrWentDown))
                                                   changeState(WidgetState.ScrollDown);
                                               else if (canvas_.imui.checkMouseButton(MouseButton.WheelUp, ButtonState.IsOrWentDown))
                                                   changeState(WidgetState.ScrollUp);
                                           }
                                       }
                                       break;
            case WidgetState.Highlighted://Hoovering over the scrollbar
                                       if (canvas_.imui.isMouseInside(displayArea_))
                                       {
                                           if (canvas_.imui.isMouseInside(currentBar_()) && canvas_.imui.checkMouseButton(MouseButton.Left, ButtonState.IsDown))
                                           {
                                               //The user pressed the left mouse button _inside_ the bar of the scrollbar
                                               //We record the displacement between the barCenter_ and the mouse position
                                               displacement_ = barCenter_ - transformReverseLinTrans(canvas_.imui.mousePosition_.y, linTrans_);
                                               changeState(WidgetState.Moving);
                                           }
                                           else
                                           {
                                               //Check for scrolling on the scrollbar
                                               if (canvas_.imui.checkMouseButton(MouseButton.WheelDown, ButtonState.IsOrWentDown))
                                                   changeState(WidgetState.ScrollDown);
                                               else if (canvas_.imui.checkMouseButton(MouseButton.WheelUp, ButtonState.IsOrWentDown))
                                                   changeState(WidgetState.ScrollUp);
                                           }
                                       }
                                       else
                                           changeState(WidgetState.Idle);
                                       break;
            case WidgetState.Moving://Button-down on the bar inside the scrollbar
                                       if (canvas_.imui.checkMouseButton(MouseButton.Left, ButtonState.IsUp))
                                       {
                                           if (canvas_.imui.isMouseInside(displayArea_))
                                               changeState(WidgetState.Highlighted);
                                           else
                                               changeState(WidgetState.Idle);
                                       }
                                       else
                                           barCenter_ = transformReverseLinTrans(canvas_.imui.mousePosition_.y, linTrans_) + displacement_;
                                       break;
            case WidgetState.ScrollDown://Scrolling inside the scrollbar or the listen area
            case WidgetState.ScrollUp:
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
            case WidgetState.Idle: s.fill(Color.purple); break;
            case WidgetState.Highlighted: s.fill(Color.coolGreen); break;
            case WidgetState.Moving: s.fill(Color.yellow); break;
            default: break;
        }
        
        canvas_.drawRectangle(currentBar_(), s);
        return state;
    }

    private:
    TwoPoint currentBar_()
    {
        return TwoPoint.centered(displayArea_.centerX, transformLinTrans(barCenter_, linTrans_), displayArea_.width, barSize_*linTrans_[0]);
    }

    TwoPoint displayArea_;
    TwoPoint listenArea_;
    real[2] linTrans_;
    real[2] range_;
    real barCenter_;
    real barSize_;
    real displacement_;
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
    if (Key.A <= key && key <= Key.Z)
        return cast(char)('A'+key-Key.A);
    switch (key)
    {
        case Key.Return: return '\n';
        case Key.Colon: return ':';
        case Key.Semicolon: return ';';
        case Key.Comma: return ',';
        default: return '.';
    }
    assert(false);
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

    A = SDLK_a - SDLK_a + 'A',
    B = SDLK_b - SDLK_a + 'A',
    C = SDLK_c - SDLK_a + 'A',
    D = SDLK_d - SDLK_a + 'A',
    E = SDLK_e - SDLK_a + 'A',
    F = SDLK_f - SDLK_a + 'A',
    G = SDLK_g - SDLK_a + 'A',
    H = SDLK_h - SDLK_a + 'A',
    I = SDLK_i - SDLK_a + 'A',
    J = SDLK_j - SDLK_a + 'A',
    K = SDLK_k - SDLK_a + 'A',
    L = SDLK_l - SDLK_a + 'A',
    M = SDLK_m - SDLK_a + 'A',
    N = SDLK_n - SDLK_a + 'A',
    O = SDLK_o - SDLK_a + 'A',
    P = SDLK_p - SDLK_a + 'A',
    Q = SDLK_q - SDLK_a + 'A',
    R = SDLK_r - SDLK_a + 'A',
    S = SDLK_s - SDLK_a + 'A',
    T = SDLK_t - SDLK_a + 'A',
    U = SDLK_u - SDLK_a + 'A',
    V = SDLK_v - SDLK_a + 'A',
    W = SDLK_w - SDLK_a + 'A',
    X = SDLK_x - SDLK_a + 'A',
    Y = SDLK_y - SDLK_a + 'A',
    Z = SDLK_z - SDLK_a + 'A',

    Return = SDLK_RETURN,
    Colon = SDLK_COLON,
    Semicolon = SDLK_SEMICOLON,
    Comma = SDLK_COMMA,

    Up = SDLK_UP,
    Down = SDLK_DOWN,
    Right = SDLK_RIGHT,
    Left = SDLK_LEFT,

    Escape = SDLK_ESCAPE,
}
enum MouseButton
{
    Left,
    Middle,
    Right,
    WheelUp,
    WheelDown,
}
enum ButtonState
{
    IsUp,
    IsDown,
    IsOrWentUp,
    IsOrWentDown,
}

//Immediate-mode user interface
abstract class IMUI
{
    //Call this fast enough to get a reasonable response time
    //Returns true if some event is waiting (not sure if this maps OK for all input devices)
    //Calling this twice without getting any event should return the same
    final bool processInput()
    {
        if (processInput_())
            somethingChanged_ = true;
        return somethingChanged_;
    }
    //Resets the changed status, which should be done after one iteration through the main loop
    void reset(){somethingChanged_ = false;}
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
    bool checkMouseButton(MouseButton button, ButtonState cmpState)
    {
        bool isUp, changed;
        switch (button)
        {
            case MouseButton.Left: leftMouse_.get(isUp, changed); break;
            case MouseButton.Middle: middleMouse_.get(isUp, changed); break;
            case MouseButton.Right: rightMouse_.get(isUp, changed); break;
            case MouseButton.WheelUp: wheelUp_.get(isUp, changed); break;
            case MouseButton.WheelDown: wheelDown_.get(isUp, changed); break;
        }
        switch (cmpState)
        {
            case ButtonState.IsDown: return false == isUp; break;
            case ButtonState.IsUp: return true == isUp; break;
            case ButtonState.IsOrWentDown: return false == isUp || changed; break;
            case ButtonState.IsOrWentUp: return true == isUp || changed; break;
        }
        //Should never be reached
        assert(false);
        return false;
    }

    protected:
    bool somethingChanged_ = false;
    //This should be provided by any derived class, implementing basic input processing
    //Keeping track of input changes is done by processInput()
    bool processInput_();

    Key lastKey_ = Key.None;
    //When processInput() polls for key presses, it can store excess Keys in here and move them
    //later to lastKey_ one by one
    Key[] cachedKeys_;
    Point mousePosition_ = Point(0.0, 0.0);
    struct ButtonHistory
    {
        void setUp(bool b)
        {
            if (isDown_ == b)
                changed_ = true;
            isDown_ = !b;
        }
        void get(ref bool cur, ref bool ch)
        {
            cur = !isDown_;
            ch = changed_;
            changed_ = false;
        }
        private:
        bool isDown_;
        bool changed_;
    }
    ButtonHistory leftMouse_;
    ButtonHistory middleMouse_;
    ButtonHistory rightMouse_;
    ButtonHistory wheelUp_;
    ButtonHistory wheelDown_;
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
