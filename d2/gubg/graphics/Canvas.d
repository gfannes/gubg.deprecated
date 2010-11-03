module gubg.graphics.Canvas;

import gubg.Point;
import gubg.graphics.Style;
import gubg.graphics.Cairo;
import derelict.sdl.sdl;
import std.stdio;

interface ICanvas
{
    //Called once or more before any other call
    bool initialize();
    bool finalize();

    //Called before and after each draw session
    bool initializeDraw();
    void finalizeDraw();

    //width and height of canvas
    //(0,0) is bottom left and (width-1, height-1) is top right
    int width();
    int height();

    //Drawing primitives
    void drawLine(TwoPoint fromTo, Style);
    void drawCircle(Point center, real radius, Style);
    void drawRectangle(TwoPoint fromTo, Style);
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
        auto s = Style().fill(Color.green).stroke(Color.red);
        foreach (i; 0 .. 240)
        {
            canvas.initializeDraw;
            s.width(i/5+1);
            canvas.drawCircle(Point(i, i), i, s);
            canvas.finalizeDraw;
            Thread.sleep(100000);
        }
        canvas.finalize();
    }
}
