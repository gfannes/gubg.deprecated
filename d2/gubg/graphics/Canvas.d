module gubg.graphics.Canvas;

public import gubg.Point;
public import gubg.graphics.Style;
import gubg.graphics.Cairo;
import derelict.sdl.sdl;
import std.stdio;

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
interface ICanvas
{
    //Called once or more before any other call
    bool initialize();
    bool finalize();

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
}

class SDLCanvas: ICanvas
{
    this(int width, int height)
    {
        width_ = width;
        height_ = height;
        initialized_ = false;
        SDLSurface_ = null;
    }

    public:
    bool initialize()
    {
        if (initialized_)
            return true;

        DerelictSDL.load();
        //Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            writeln(stderr, "Unable to init SDL: %s\n", SDL_GetError());
            SDL_Quit();
            return false;
        }
        //Create the screen surface (window)
        SDLSurface_ = SDL_SetVideoMode(width_, height_, 32, SDL_SWSURFACE);
        if (SDLSurface_ is null)
        {
            writeln(stderr, "Unable to set %s x %s video: %s\n", width_, height_, SDL_GetError());
            SDL_Quit();
            return false;
        }

        cairo_ = new gubg.graphics.Cairo.Context(cast(ubyte*)(SDLSurface_.pixels), width_, height_);

        lastKey_ = SDLK_UNKNOWN;
        initialized_ = true;
        return true;
    }
    bool finalize()
    {
        if (initialized_)
            SDL_Quit();
        return true;
    }

    bool initializeDraw()
    {
        if (SDL_MUSTLOCK(SDLSurface_))
            if (SDL_LockSurface(SDLSurface_) < 0)
                return false;
        clear();

        //Collect event
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    lastKey_ = event.key.keysym.sym;
                    break;
                default: break;
            }
        }
        return true;
    }
    void finalizeDraw()
    {
        if (SDL_MUSTLOCK(SDLSurface_))
            SDL_UnlockSurface(SDLSurface_);
        SDL_UpdateRect(SDLSurface_, 0, 0, width_, height_);
    }

    int width(){return width_;}
    int height(){return height_;}

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
    void drawText(string text, TwoPoint box, VAlign valign, HAlign halign, Style style)
    {
        setStrokeStyle_(style);

        //Compute and set the font size
        // * Compute the for font size 1, watching out for roundings generated by cairo
        real descentFrac, ascentFrac, heightFrac, caplineFrac;
        {
            const tmpFontSize = 1000.0;
            cairo_.setFontSize(tmpFontSize);
            cairo_.fontExtents(ascentFrac, descentFrac, heightFrac);
            ascentFrac /= tmpFontSize;
            descentFrac /= tmpFontSize;
            heightFrac /= tmpFontSize;
            caplineFrac = 1.0 - descentFrac;
        }
        switch (valign)
        {
            case VAlign.Bottom:
                real fontSize = box.height/heightFrac;
                cairo_.setFontSize(fontSize);
                cairo_.moveTo(box.p0.x, height_-1-box.p0.y);
                break;
            case VAlign.Center:
                real fontSize = box.height;
                cairo_.setFontSize(fontSize);
                cairo_.moveTo(box.p0.x, height_-1-box.p0.y-descentFrac*fontSize);
                break;
        }
        cairo_.showText(text);
        cairo_.stroke();
    }

    bool getNumericKey(out uint number)
    {
        if (SDLK_UNKNOWN == lastKey_)
            return false;

        if (SDLK_0 <= lastKey_ && lastKey_ <= SDLK_9)
        {
            number = lastKey_ - SDLK_0;
            lastKey_ = SDLK_UNKNOWN;
            return true;
        }
        if (SDLK_KP0 <= lastKey_ && lastKey_ <= SDLK_KP9)
        {
            number = lastKey_ - SDLK_KP0;
            lastKey_ = SDLK_UNKNOWN;
            return true;
        }
        return false;
    }
    bool escapeIsPressed()
    {
        if (SDLK_UNKNOWN == lastKey_)
            return false;
        if (SDLK_ESCAPE == lastKey_)
        {
            lastKey_ = SDLK_UNKNOWN;
            return true;
        }
        return false;
    }

    private:
    void clear(uint rgb = 0x123456){SDL_FillRect(SDLSurface_, null, rgb);}
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
    gubg.graphics.Cairo.Context cairo_;
    SDLKey lastKey_;

    int width_;
    int height_;
    bool initialized_;
}

version (UnitTest)
{
    import std.stdio;
    import core.thread;
    void main()
    {
        auto canvas = new SDLCanvas(640, 480);
        canvas.initialize();
        auto sc = Style().fill(Color.green).stroke(Color.red);
        auto sr = Style().fill(Color.blue);
        canvas.setFont("serif");
        foreach (i; 0 .. 240)
        {
            scope ds = canvas.new DrawScope;
            sc.width(i/5+1);
            canvas.drawCircle(Point(i, i), i, sc);
            auto half = 0.5*i;
            auto box = TwoPoint(i-half, i-half, i+half, i+half);
            canvas.drawRectangle(box, sr);
            canvas.drawText("pgjpqyl|", box, VAlign.Center, HAlign.Left, sc);
            Thread.sleep(100000);
        }
        Thread.sleep(100000000);
        canvas.finalize();
    }
}
