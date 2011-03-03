#ifndef gubg_graphics_style_hpp
#define gubg_graphics_style_hpp

#include <ostream>
#include <array>
#include <cassert>

namespace gubg
{
    struct Color
    {
        // Some predefined colors
        static const Color black;
        static const Color white;
        static const Color red;
        static const Color green;
        static const Color blue;
        static const Color purple;
        static const Color cyan;
        static const Color yellow;
        static const Color gray;
        static const Color coolRed;
        static const Color darkRed;
        static const Color coolGreen;
        static const Color darkGreen;
        static const Color coolBlue;
        static const Color darkBlue;
        static const Color invalid;

        //Constructors
        Color();
        Color(double rr, double gg, double bb):
            r(rr), g(gg), b(bb){}
        Color(const std::array<double, 3> &rgb):
            r(rgb[0]), g(rgb[1]), b(rgb[2]){}
        Color(double shade):
            r(0), g(0), b(0)
        {
            if (shade > 0)
                g = shade;
            else
                r = -shade;
        }

        // Data fields
        double r;
        double g;
        double b;

        unsigned char redByte() const {return 255*r;}
        unsigned char greenByte() const {return 255*g;}
        unsigned char blueByte() const {return 255*b;}
        unsigned int rgb() const {return (redByte()<<16) | (greenByte()<<8) | (blueByte());}

        bool isValid() const;
    };
}
std::ostream &operator<<(std::ostream &os, const gubg::Color &color);

namespace gubg
{
    struct Style;
}
std::ostream &operator<<(std::ostream &os, const gubg::Style &style);
namespace gubg
{
    struct Style
    {
        static const double InvalidWidth = -1;
        Style():
            width_(InvalidWidth){}

        Style &fill(Color fill) { fill_ = fill; return *this; }
        Style &stroke(Color stroke) { stroke_ = stroke; return *this; }
        Style &width(double width) { width_ = width; return *this; }

        bool hasFill() const {return fill_.isValid();}
        bool hasStroke() const {return stroke_.isValid() && InvalidWidth != width_;}
        const Color &fillColor() const {assert(hasFill()); return fill_;}
        const Color &strokeColor() const {assert(hasStroke()); return stroke_;}
        double strokeWidth() const {assert(hasStroke()); return width_;}

        private:
        friend std::ostream &::operator<<(std::ostream &, const Style &);
        Color fill_;
        Color stroke_;
        double width_;
    };
}

#endif
