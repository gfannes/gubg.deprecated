#ifndef HEADER_gubg_graphics_sdl_canvas_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graphics_sdl_canvas_hpp_ALREADY_INCLUDED

#include "gubg/graphics/canvas.hpp"
#include "gubg/graphics/sdl.hpp"
#include "gubg/graphics/imui.hpp"
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#include "cairomm/context.h"
#include "boost/scoped_ptr.hpp"
#include <sstream>

namespace gubg
{
    class SDLCanvas: public ICanvas
    {
        public:
            SDLCanvas(int width, int height, bool useBorder = true);

            virtual bool initializeDraw();
            virtual void finalizeDraw();
            virtual int width() const;
            virtual int height() const;
            virtual void drawLine(const TwoPoint<> &fromTo, const Style &);
            virtual void drawCircle(const Point<> &center, double radius, const Style &);
            virtual void drawRectangle(const TwoPoint<> &fromTo, const Style &);
            virtual void setFont(const std::string &font);
            virtual void drawText(const std::string &text, const TwoPoint<> &box, VAlign, HAlign, const Style &);

#if 0
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
#endif
            typedef std::shared_ptr<gubg::IMUI> IMUIPtr;
            IMUIPtr getIMUI();

            void clear(const Color &color = Color::coolBlue);

        private:
            void flip(){SDL_Flip(SDLSurface_);}
            void setStrokeStyle_(const Style &);
            void setFillStyle_(const Style &);
            TwoPoint<> textBB_(const std::string &str);

        private:
            SDL_Surface *SDLSurface_;
            SDLPtr sdl_;
            Cairo::RefPtr<Cairo::ImageSurface> cairoSurface_;
            Cairo::RefPtr<Cairo::Context> cairoContext_;    

            int width_;
            int height_;
            SDL_SysWMinfo info_;

            IMUIPtr imui_;
    };
}

#if 0
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
#endif

#endif
