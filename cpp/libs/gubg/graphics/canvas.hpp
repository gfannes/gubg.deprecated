#ifndef HEADER_gubg_graphics_canvas_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graphics_canvas_hpp_ALREADY_INCLUDED

#include "graphics/style.hpp"
#include "graphics/sdl.hpp"
#include "point.hpp"

namespace gubg
{
    enum class VAlign
    {
        Bottom,//The bottom of the TwoPoint is matched with the baseline, the top with the height line
        //This is suitable for multi-line text, as line spacing is automatically handled
        Center,//The bottom of the TwoPoint is matched with the descent line, the top with the ascent line
        //This is suitable for centered single-line text, as the characters more or less fall nicely
        //in the box (given that the text contains both low and high characters like "g" and "l").
    };
    enum class HAlign
    {
        Left,
        Center,
        Right,
        Fill,//This probably only makes sense in combination with VAlign.Center
    };
    class ICanvas
    {
        public:
            //Called before and after each draw session
            virtual bool initializeDraw() = 0;
            virtual void finalizeDraw() = 0;
            struct DrawScope
            {
                DrawScope(ICanvas &canvas):
                    canvas_(canvas){canvas_.initializeDraw();}
                ~DrawScope(){canvas_.finalizeDraw();}
                ICanvas &canvas_;
            };

            //width and height of canvas
            //(0,0) is bottom left and (width-1, height-1) is top right
            virtual int width() const = 0;
            virtual int height() const = 0;

            //Drawing primitives
            virtual void drawLine(const TwoPoint<> &fromTo, const Style &) = 0;
            virtual void drawCircle(const Point<> &center, double radius, const Style &) = 0;
            virtual void drawRectangle(const TwoPoint<> &fromTo, const Style &) = 0;
            virtual void setFont(const std::string &font) = 0;
            //By default, the height of the box controls the font size, except when HAlign.Fill is chosen
//            virtual void drawText(const std::string &text, const TwoPoint<> &box, VAlign, HAlign, const Style &) = 0;
    };
}

#endif
