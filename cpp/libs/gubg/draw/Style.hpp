#ifndef gubg_draw_Style_hpp
#define gubg_draw_Style_hpp

#include "gubg/draw/Color.hpp"
#include <memory>

namespace gubg
{
    namespace draw
    {
        template <typename Color_, typename Width_>
            struct Style
            {
                    typedef Color_ Color;
                    typedef Width_ Width;
                    typedef std::unique_ptr<Color> ColorPtr;
                    typedef std::unique_ptr<Width> WidthPtr;

                    ColorPtr fill;
                    ColorPtr stroke;
                    WidthPtr width;

                    Style(){}
                    Style(Style &&s):
                        fill(std::move(s.fill)),
                        stroke(std::move(s.stroke)),
                        width(std::move(s.width)){}

                    template <typename C>
                        Style &setFill(const C &c) { fill.reset(new Color(c)); return *this; }
                    template <typename C>
                        Style &setStroke(const C &c) { stroke.reset(new Color(c)); return *this; }
                    template <typename W>
                        Style &setWidth(const W &c) { width.reset(new Width(c)); return *this; }

            };
    }
}

#endif
