module gubg.graphics.Canvas;

public import gubg.Point;
public import gubg.graphics.Style;
import gubg.graphics.SDL;
import gubg.graphics.Cairo;
import gubg.graphics.IMUI;
import gubg.Format;
import derelict.sdl.sdl;
import std.array;
import std.string;
import std.md5;

import std.stdio;

//MD5 hashing is too slow to get a benefit, try some non-cryptographical, but still close to universal hashing function to get better performance
//version=usecache;

bool verbose__ = false;

enum VAlign
{
    Bottom,//The bottom of the TwoPoint is matched with the baseline, the top with the height line
    //This is suitable for multi-line text, as line spacing is automatically handled
    Center,//The bottom of the TwoPoint is matched with the descent line, the top with the ascent line
    //This is suitable for centered single-line text, as the characters more or less fall nicely
    //in the box (given that the text contains both low and high characters like "g" and "l").
}
enum HAlign
{
    Left,
    Center,
    Right,
    Fill,//This probably only makes sense in combination with VAlign.Center
}
struct MarkupString
{
    this(string str, Color c)
    {
        strings = [str];
        colors = [c];
    }
    MarkupString add(string str, Color c)
    {
        strings ~= str;
        colors ~= c;
        return this;
    }
    string text()
    {
        string res = "";
        foreach (str; strings)
            res ~= str;
        return res;
    }
    int opApply(int delegate(ref string, ref Color) dg)
    {
        foreach (ix, ref str; strings)
        {
            auto res = dg(str, colors[ix]);
            if (res)
                return res;
        }
        return 0;
    }
    string[] strings;
    Color[] colors;
}
interface ICanvas
{
    //Called before and after each draw session
    bool initializeDraw();
    void finalizeDraw();
    scope class DrawScope
    {
        this () { ok_ = initializeDraw(); }
        ~this () { if (ok_) finalizeDraw(); }
        private: bool ok_ = false;
    }

    //width and height of canvas
    //(0,0) is bottom left and (width-1, height-1) is top right
    int width();
    int height();

    //Drawing primitives
    void drawLine(TwoPoint fromTo, Style);
    void drawCircle(Point center, real radius, Style);
    void drawRectangle(TwoPoint fromTo, Style);
    void setFont(string font);
    //By default, the height of the box controls the font size, except when HAlign.Fill is chosen
    void drawText(string text, TwoPoint box, VAlign, HAlign, Style);
    void drawText(MarkupString ms, TwoPoint box, VAlign, HAlign, Style);
}

class SDLCanvas: ICanvas
{
    this (int width, int height, bool useBorder = true)
    {
        width_ = width;
        height_ = height;

        //Lazy initialization of SDL
        if (sdl_ is null)
            sdl_ = new SDL;

        //Create the screen surface (window)
        auto flags = SDL_SWSURFACE;
        if (!useBorder)
            flags |= SDL_NOFRAME;
        SDLSurface_ = SDL_SetVideoMode(width_, height_, 32, flags);
        if (SDLSurface_ is null)
            throw new Exception(Format.immediate("Unable to set video mode to %s x %s: %s", width_, height_, SDL_GetError()));

        SDL_VERSION(&info_.ver);
        SDL_GetWMInfo(&info_);

        cairo_ = new gubg.graphics.Cairo.Context(cast(ubyte*)(SDLSurface_.pixels), width_, height_);
    }
    void setTitle(string windowTitle, string iconTitle)
    {
        SDL_WM_SetCaption(toStringz(windowTitle), toStringz(iconTitle));
    }
    void setIcon(string iconPath, Color alphaColor = Color.invalid)
    {
        auto icon = SDL_LoadBMP(toStringz(iconPath));
        if (alphaColor.isValid)
        {
            auto colorkey = SDL_MapRGB(icon.format, alphaColor.redByte, alphaColor.greenByte, alphaColor.blueByte);
            SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colorkey);              
        }
        SDL_WM_SetIcon(icon, null);
    }

    bool initializeDraw()
    {
        if (SDL_MUSTLOCK(SDLSurface_))
            if (SDL_LockSurface(SDLSurface_) < 0)
                return false;
        clear();

        //Only if our IMUI member is present, we will process input
        if (!(imui_ is null))
            imui_.processInput();

        return true;
    }
    void finalizeDraw()
    {
        //Only if our IMUI member is present, we will process input
        if (!(imui_ is null))
            imui_.reset();

        if (SDL_MUSTLOCK(SDLSurface_))
            SDL_UnlockSurface(SDLSurface_);
        //SDL_UpdateRect should _not_ be called inside the lock
        SDL_UpdateRect(SDLSurface_, 0, 0, width_, height_);
    }

    int width() const {return width_;}
    int height() const {return height_;}

    void drawLine(TwoPoint fromTo, Style strokeStyle)
    {
        if (strokeStyle.hasStroke)
        {
            setStrokeStyle_(strokeStyle);
            cairo_.moveTo(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairo_.lineTo(fromTo.p1.x, height_-1-fromTo.p1.y);
            cairo_.stroke();
        }
    }
    void drawCircle(Point center, real radius, Style style)
    {
        if (style.hasFill)
        {
            setFillStyle_(style);
            cairo_.arc(center.x, height_-1-center.y, radius, 0.0, 6.28318530717959);
            cairo_.fill();
        }
        if (style.hasStroke)
        {
            setStrokeStyle_(style);
            cairo_.arc(center.x, height_-1-center.y, radius, 0.0, 6.28318530717959);
            cairo_.stroke();
        }
    }
    void drawRectangle(TwoPoint fromTo, Style style)
    {
        if (style.hasFill)
        {
            setFillStyle_(style);
            cairo_.moveTo(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairo_.lineTo(fromTo.p0.x, height_-1-fromTo.p1.y);
            cairo_.lineTo(fromTo.p1.x, height_-1-fromTo.p1.y);
            cairo_.lineTo(fromTo.p1.x, height_-1-fromTo.p0.y);
            cairo_.lineTo(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairo_.fill();
        }
        if (style.hasStroke)
        {
            setStrokeStyle_(style);
            cairo_.moveTo(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairo_.lineTo(fromTo.p0.x, height_-1-fromTo.p1.y);
            cairo_.lineTo(fromTo.p1.x, height_-1-fromTo.p1.y);
            cairo_.lineTo(fromTo.p1.x, height_-1-fromTo.p0.y);
            cairo_.lineTo(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairo_.stroke();
        }
    }
    void setFont(string font)
    {
        cairo_.selectFontFace(font);
    }

    private struct FontInfo
    {
        real fontSize;
        real x;
        real y;
    }
    private class FontInfoCache
    {
        version (usecache)
        {
            alias ubyte[16] Hash;
            MD5_CTX md5_;
            FontInfo[Hash] fontInfoCache_;
        }
        void getInfo(ref FontInfo fontInfo, string text, TwoPoint box, VAlign valign, HAlign halign, real strokeWidth)
        {
            version (usecache)
            {
                md5_.start();
                md5_.update(text);
                md5_.update(Format.immediate("%s %s %s %s", box, valign, halign, strokeWidth));
                Hash h;
                md5_.finish(h);
                if (FontInfo *info = h in fontInfoCache_)
                {
                    fontInfo = *info;
                    return;
                }
            }
            cairo_.setLineWidth(strokeWidth);

            //Compute and set the font size
            // * Compute the different font properties for font size 1,
            //watching out for roundings generated by cairo
            // * We assume that the strokeWidth has neglectible influence
            static real descentFrac;
            static real ascentFrac;
            static real heightFrac;
            static real caplineFrac;
            static bool fracsAreInitialized;
            if (!fracsAreInitialized)
            {
                const tmpFontSize = 1000.0;
                cairo_.setFontSize(tmpFontSize);
                cairo_.fontExtents(ascentFrac, descentFrac, heightFrac);
                ascentFrac /= tmpFontSize;
                descentFrac /= tmpFontSize;
                heightFrac /= tmpFontSize;
                caplineFrac = 1.0 - descentFrac;
                fracsAreInitialized = true;
            }
            switch (valign)
            {
                case VAlign.Bottom:
                    fontInfo.fontSize = box.height/heightFrac;
                    fontInfo.y = box.p0.y;
                    break;
                case VAlign.Center:
                    fontInfo.fontSize = box.height;
                    fontInfo.y = box.p0.y+descentFrac*fontInfo.fontSize;
                    break;
            }
            switch (halign)
            {
                case HAlign.Left:
                    fontInfo.x = box.p0.x;
                    break;
                case HAlign.Center:
                case HAlign.Right:
                case HAlign.Fill:
                    cairo_.setFontSize(fontInfo.fontSize);
                    auto bb = cairo_.textBB(text);
                    real textWidth = bb.width();
                    switch (halign)
                    {
                        case HAlign.Center:
                            fontInfo.x = box.centerX()-0.5*textWidth;
                            break;
                        case HAlign.Right:
                            fontInfo.x = box.p1.x-textWidth;
                            break;
                        case HAlign.Fill:
                            fontInfo.fontSize *= box.width()/bb.width();
                            fontInfo.x = box.p0.x;
                            //y might also need an adjustment
                            switch (valign)
                            {
                                //For VAlign.Bottom, y does not change
                                case VAlign.Bottom: break;
                                case VAlign.Center:
                                                    fontInfo.y = box.p0.y+descentFrac*fontInfo.fontSize;
                                                    break;
                            }
                            break;
                    }
                    break;
            }
            version (usecache)
            {
                fontInfoCache_[h] = fontInfo;
            }
        }
    }
    private FontInfoCache fontInfoCache_;

    private void prepareDrawText_(string text, TwoPoint box, VAlign valign, HAlign halign, Style style)
    {
        if (fontInfoCache_ is null)
            fontInfoCache_ = new FontInfoCache();
        FontInfo fi;
        fontInfoCache_.getInfo(fi, text, box, valign, halign, style.strokeWidth);
        setStrokeStyle_(style);
        cairo_.setFontSize(fi.fontSize);
        cairo_.moveTo(fi.x, height_-1-fi.y);
    }
    void drawText(string text, TwoPoint box, VAlign valign, HAlign halign, Style style)
    {
        prepareDrawText_(text, box, valign, halign, style);
        cairo_.showText(text);
        cairo_.stroke();
    }
    void drawText(MarkupString ms, TwoPoint box, VAlign valign, HAlign halign, Style style)
    {
        prepareDrawText_(ms.text(), box, valign, halign, style);
        foreach (str, color; ms)
        {
            style.stroke(color);
            setStrokeStyle_(style);
            cairo_.showText(str);
        }
        cairo_.stroke();
    }
    void clear(uint rgb = 0x123456){SDL_FillRect(SDLSurface_, null, rgb);}
    version (Posix)
    {
        uint windowId(){return info_.windowId;}
    }

    //Immediate-mode user interface handling
    gubg.graphics.IMUI.IMUI imui()
    {
        //Lazy creation of imui_
        if (imui_ is null)
        {
            imui_ = new IMUI;
            SDL_EnableKeyRepeat(200, 50);
        }
        return imui_;
    }
    private:
    //An inner class that handles user input to the SDLCanvas
    //using immediate-mode user interface handling
    class IMUI: gubg.graphics.IMUI.IMUI
    {
        protected bool processInput_()
        {
            bool somethingChanged = false;
            //Keep polling events until the event queue is empty. If we do this one by one,
            //we get a delay in the mouse behaviour
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_KEYDOWN:
                        if (event.key.keysym.unicode != 0)
                        {
                            cachedKeys_ ~= fromSDL(event.key.keysym.unicode);
                            if (verbose__)
                                writefln("Unicode: %s", cachedKeys_[$-1]);
                        }
                        else
                        {
                            auto sdlMod = event.key.keysym.mod;
                            auto sdlKey = event.key.keysym.sym;
                            switch (sdlKey)
                            {
                                case SDLK_UP:
                                case SDLK_DOWN:
                                case SDLK_PAGEUP:
                                case SDLK_PAGEDOWN:
                                case SDLK_RIGHT:
                                case SDLK_LEFT:
                                case SDLK_F1:
                                case SDLK_F2:
                                case SDLK_F3:
                                case SDLK_F4:
                                case SDLK_F5:
                                case SDLK_F6:
                                case SDLK_F7:
                                case SDLK_F8:
                                case SDLK_F9:
                                case SDLK_F10:
                                case SDLK_F11:
                                case SDLK_F12:
                                case SDLK_F13:
                                case SDLK_F14:
                                case SDLK_F15:
                                case SDLK_DELETE:
                                    cachedKeys_ ~= fromSDL(sdlKey, sdlMod);
                                    break;
                                default: break;
                            }
                        }
                        break;
                    case SDL_MOUSEMOTION:
                        mousePosition_.x = event.motion.x;
                        mousePosition_.y = height() - event.motion.y - 1;
                        somethingChanged = true;
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        somethingChanged = true;
                        switch (event.button.button)
                        {
                            case SDL_BUTTON_LEFT: leftMouse_.setUp(false); break;
                            case SDL_BUTTON_MIDDLE: middleMouse_.setUp(false); break;
                            case SDL_BUTTON_RIGHT: rightMouse_.setUp(false); break;
                            case SDL_BUTTON_WHEELUP: wheelUp_.setUp(false); break;
                            case SDL_BUTTON_WHEELDOWN: wheelDown_.setUp(false); break;
                            default: somethingChanged = false; break;
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        somethingChanged = true;
                        switch (event.button.button)
                        {
                            case SDL_BUTTON_LEFT: leftMouse_.setUp(true); break;
                            case SDL_BUTTON_MIDDLE: middleMouse_.setUp(true); break;
                            case SDL_BUTTON_RIGHT: rightMouse_.setUp(true); break;
                            case SDL_BUTTON_WHEELUP: wheelUp_.setUp(true); break;
                            case SDL_BUTTON_WHEELDOWN: wheelDown_.setUp(true); break;
                            default: somethingChanged = false; break;
                        }
                        break;

                    default: break;
                }
            }
            if (Key.None == lastKey_ && !cachedKeys_.empty())
            {
                lastKey_ = cachedKeys_.front();
                cachedKeys_.popFront();
            }
            if (Key.None != lastKey_)
                somethingChanged = true;
            return somethingChanged;
        }
    }
    IMUI imui_;

    private:
    void flip(){SDL_Flip(SDLSurface_);}
    void setStrokeStyle_(Style style)
    {
        cairo_.setLineWidth(style.strokeWidth);
        cairo_.setSourceRGB(style.strokeColor.r, style.strokeColor.g, style.strokeColor.b);
    }
    void setFillStyle_(Style style)
    {
        cairo_.setSourceRGB(style.fillColor.r, style.fillColor.g, style.fillColor.b);
    }

    private:
    SDL_Surface* SDLSurface_;
    gubg.graphics.SDL.SDL sdl_;
    gubg.graphics.Cairo.Context cairo_;

    int width_;
    int height_;
    SDL_SysWMinfo info_;
}

version (UnitTest)
{
    import std.stdio;
    import std.process;
    import core.thread;
    void main()
    {
        enum ETest {Text, Multiline, MPlayer, All};
        ETest test = ETest.MPlayer;

        scope canvas = new SDLCanvas(1024, 480, false);
        ICanvas canvas2;
        auto sc = Style().fill(Color.green).stroke(Color.red);
        auto sr = Style();
        canvas.setFont("serif");
        foreach (i; 0 .. 24000)
        {
            scope ds = canvas.new DrawScope;
            if (canvas.imui.escapeIsPressed)
                break;
            switch (test)
            {
                case ETest.Text:
                    sr.stroke(Color.yellow).width(2);
                    auto vAligns = [VAlign.Bottom, VAlign.Center];
                    auto hAligns = [HAlign.Left, HAlign.Center, HAlign.Right, HAlign.Fill];
                    sc.width(5);
                    auto w = 150.0;
                    auto h = 50.0;
                    foreach (ixh, hAlign; hAligns)
                    {
                        foreach (ixv, vAlign; vAligns)
                        {
                            auto box = TwoPoint(10.0+w*ixh, 100.0+h*ixv, 10.0+w*(0.9+ixh), 100.0+h*(0.9+ixv));
                            canvas.drawRectangle(box, sr);
                            canvas.drawText("p|", box, vAlign, hAlign, sc);
                        }
                    }
                    break;
                case ETest.Multiline:
                    TwoPoint[] boxes = new TwoPoint[10];
                    foreach (ix, ref box; boxes)
                        box = TwoPoint(0, 48*(9-ix), 1024, 48*(10-ix));
                    sr.stroke(Color.yellow).width(2);
                    sc.width(2);
                    string[] strAry = ["Dees ziet er al geweldig uit",
                        "Inderdaad, best wel in orde",
                        "Kzal eens een lijntje leeg laten si",
                        "",
                        "Hey, waar is de vorige lijn naar toe?",
                        "Khad het u toch gezegd,",
                        "Kging een lijntje leeg laten",
                        "",
                        "Doven otter",
                        "Schelden is voor nix nodig!"];
                    foreach (ix, box; boxes)
                    {
                        canvas.drawRectangle(box, sr);
                        canvas.drawText(strAry[ix], box, VAlign.Bottom, HAlign.Left, sc);
                    }
                    break;
                case ETest.MPlayer:
                    shell(Format.immediate("mplayer -ss 10 -frames 100 -wid %s ~/kwis/Films/raw/Addams\\ Family\\ Movie\\ Trailer.flv", canvas.windowId));
                    break;
                case ETest.All:
                    sr.fill(Color.blue);
                    sc.width(i/5+1);
                    canvas.drawCircle(Point(i, i), i, sc);
                    auto half = 0.5*i;
                    auto box = TwoPoint(i-half, i-half, i+half, i+half);
                    canvas.drawRectangle(box, sr);
                    canvas.drawText("pgjpqyl|", box, VAlign.Center, HAlign.Left, sc);
                    break;
            }
            Thread.sleep(100000);
        }
    }
}
