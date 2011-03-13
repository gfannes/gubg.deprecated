#include "graphics/sdl_canvas.hpp"
#include "graphics/imui_key.hpp"
#include "cairomm/enums.h"
#include <exception>
#include <iostream>
using namespace std;

#define L_ENABLE_DEBUG
#include "debug/debug.hpp"

namespace
{
    bool verbose__ = true;
    //An inner class that handles user input to the SDLCanvas
    //using immediate-mode user interface handling
    struct LIMUI: gubg::IMUI
    {
        public:
            LIMUI(gubg::ICanvas &canvas):
                canvas_(canvas){}

        protected:
        virtual bool processInput_()
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
                            cachedKeys_.push_back(gubg::fromSDL(event.key.keysym.unicode));
                            if (verbose__)
                                cout << "Unicode: " << cachedKeys_.back() << endl;
                        }
                        else
                        {
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
                                    cachedKeys_.push_back(gubg::fromSDL(sdlKey));
                                    break;
                                default: break;
                            }
                        }
                        break;
                    case SDL_MOUSEMOTION:
                        mousePosition_.x = event.motion.x;
                        mousePosition_.y = canvas_.height() - event.motion.y - 1;
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
            if (gubg::Key::Nil == lastKey_ && !cachedKeys_.empty())
            {
                lastKey_ = cachedKeys_.front();
                cachedKeys_.pop_front();
            }
            if (gubg::Key::Nil != lastKey_)
                somethingChanged = true;
            return somethingChanged;
        }

        private:
        gubg::ICanvas &canvas_;
    };
}

namespace gubg
{
    SDLCanvas::SDLCanvas(int width, int height, bool useBorder):
        width_(width),
        height_(height),
        sdl_(SDL::initialize())
    {
        //Create the screen surface (window)
        auto flags = SDL_SWSURFACE;
        if (!useBorder)
            flags |= SDL_NOFRAME;
        SDLSurface_ = SDL_SetVideoMode(width_, height_, 32, flags);
        if (!SDLSurface_)
        {
            std::ostringstream oss;
            oss << "Unable to set video mode to " << width_ << " x " << height_ << ": " << SDL_GetError();
            throw new std::runtime_error(oss.str());
        }

        SDL_VERSION(&info_.version);
        SDL_GetWMInfo(&info_);

        cairoSurface_ = Cairo::ImageSurface::create((unsigned char *)SDLSurface_->pixels, Cairo::FORMAT_ARGB32, width_, height_, width_*4);
        cairoContext_ = Cairo::Context::create(cairoSurface_);
        DEBUG_PRINT("SDLCanvas is constructed");
    }

    bool SDLCanvas::initializeDraw()
    {
        if (SDL_MUSTLOCK(SDLSurface_))
            if (SDL_LockSurface(SDLSurface_) < 0)
                return false;
        clear();

        //Only if our IMUI member is present, we will process input
        if (imui_)
            imui_->processInput();

        return true;
    }
    void SDLCanvas::finalizeDraw()
    {
        //Only if our IMUI member is present, we will process input
        if (imui_)
            imui_->reset();

        if (SDL_MUSTLOCK(SDLSurface_))
            SDL_UnlockSurface(SDLSurface_);
        //SDL_UpdateRect should _not_ be called inside the lock
        SDL_UpdateRect(SDLSurface_, 0, 0, width_, height_);
    }

    int SDLCanvas::width() const {return width_;}
    int SDLCanvas::height() const {return height_;}

    void SDLCanvas::drawLine(const TwoPoint<> &fromTo, const Style &strokeStyle)
    {
        if (strokeStyle.hasStroke())
        {
            setStrokeStyle_(strokeStyle);
            cairoContext_->move_to(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairoContext_->line_to(fromTo.p1.x, height_-1-fromTo.p1.y);
            cairoContext_->stroke();
        }
    }
    void SDLCanvas::drawCircle(const Point<> &center, double radius, const Style &style)
    {
        if (style.hasFill())
        {
            setFillStyle_(style);
            cairoContext_->arc(center.x, height_-1-center.y, radius, 0.0, 6.28318530717959);
            cairoContext_->fill();
        }
        if (style.hasStroke())
        {
            setStrokeStyle_(style);
            cairoContext_->arc(center.x, height_-1-center.y, radius, 0.0, 6.28318530717959);
            cairoContext_->stroke();
        }
    }
    void SDLCanvas::drawRectangle(const TwoPoint<> &fromTo, const Style &style)
    {
        if (style.hasFill())
        {
            setFillStyle_(style);
            cairoContext_->move_to(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairoContext_->line_to(fromTo.p0.x, height_-1-fromTo.p1.y);
            cairoContext_->line_to(fromTo.p1.x, height_-1-fromTo.p1.y);
            cairoContext_->line_to(fromTo.p1.x, height_-1-fromTo.p0.y);
            cairoContext_->line_to(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairoContext_->fill();
        }
        if (style.hasStroke())
        {
            setStrokeStyle_(style);
            cairoContext_->move_to(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairoContext_->line_to(fromTo.p0.x, height_-1-fromTo.p1.y);
            cairoContext_->line_to(fromTo.p1.x, height_-1-fromTo.p1.y);
            cairoContext_->line_to(fromTo.p1.x, height_-1-fromTo.p0.y);
            cairoContext_->line_to(fromTo.p0.x, height_-1-fromTo.p0.y);
            cairoContext_->stroke();
        }
    }
    void SDLCanvas::setFont(const std::string &font)
    {
        cairoContext_->select_font_face(font, Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    }
    void SDLCanvas::drawText(const std::string &text, const TwoPoint<> &box, VAlign valign, HAlign halign, const Style &style)
    {
        setStrokeStyle_(style);

        //Compute and set the font size
        // * Compute the different font properties for font size 1,
        //watching out for roundings generated by cairo
        Cairo::FontExtents fontExtents;
        double descentFrac, ascentFrac, heightFrac, caplineFrac;
        {
            const double tmpFontSize = 1000.0;
            cairoContext_->set_font_size(tmpFontSize);
            cairoContext_->get_font_extents(fontExtents);
            ascentFrac = fontExtents.ascent/tmpFontSize;
            descentFrac = fontExtents.descent/tmpFontSize;
            heightFrac = fontExtents.height/tmpFontSize;
            caplineFrac = 1.0 - descentFrac;
        }
        double fontSize;
        double y;
        switch (valign)
        {
            case VAlign::Bottom:
                fontSize = box.height()/heightFrac;
                y = box.p0.y;
                break;
            case VAlign::Center:
                fontSize = box.height();
                y = box.p0.y+descentFrac*fontSize;
                break;
        }
        cairoContext_->set_font_size(fontSize);
        double x;
        switch (halign)
        {
            case HAlign::Left:
                x = box.p0.x;
                break;
            case HAlign::Center:
            case HAlign::Right:
            case HAlign::Fill:
                auto bb = textBB_(text);
                double textWidth = bb.width();
                switch (halign)
                {
                    case HAlign::Center:
                        x = box.centerX()-0.5*textWidth;
                        break;
                    case HAlign::Right:
                        x = box.p1.x-textWidth;
                        break;
                    case HAlign::Fill:
                        fontSize *= box.width()/bb.width();
                        cairoContext_->set_font_size(fontSize);
                        x = box.p0.x;
                        //y might also need an adjustment
                        switch (valign)
                        {
                            //For VAlign.Bottom, y does not change
                            case VAlign::Bottom: break;
                            case VAlign::Center:
                                                y = box.p0.y+descentFrac*fontSize;
                                                break;
                        }
                        break;
                }
                break;
        }
        cairoContext_->move_to(x, height_-1-y);
        cairoContext_->show_text(text);
        cairoContext_->stroke();
    }
    TwoPoint<> SDLCanvas::textBB_(const std::string &str)
    {
        Cairo::TextExtents extents;
        cairoContext_->get_text_extents(str, extents);
        return TwoPoint<>(extents.x_bearing, extents.y_bearing, extents.x_bearing+extents.width, extents.y_bearing+extents.height);
    }
    void SDLCanvas::clear(const Color &color)
    {
        SDL_FillRect(SDLSurface_, 0, color.rgb());
    }
    void SDLCanvas::setStrokeStyle_(const Style &style)
    {
        cairoContext_->set_line_width(style.strokeWidth());
        cairoContext_->set_source_rgb(style.strokeColor().r, style.strokeColor().g, style.strokeColor().b);
    }
    void SDLCanvas::setFillStyle_(const Style &style)
    {
        cairoContext_->set_source_rgb(style.fillColor().r, style.fillColor().g, style.fillColor().b);
    }
#if 0
    version (posix)
    {
        uint windowId(){return info_.windowId;}
    }
#endif

    //Immediate-mode user interface handling
    SDLCanvas::IMUIPtr SDLCanvas::getIMUI()
    {
        //Lazy creation of imui_
        if (!imui_)
        {
            imui_.reset(new LIMUI(*this));
            SDL_EnableKeyRepeat(200, 50);
        }
        return imui_;
    }
}
