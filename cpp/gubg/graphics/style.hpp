#ifndef gubg_graphics_style_hpp
#define gubg_graphics_style_hpp

#include <ostream>
#include <array>
#include <cassert>

namespace gubg
{
    struct Color;
    struct Style;
}

std::ostream &operator<<(std::ostream &os, const gubg::Color &color);
std::ostream &operator<<(std::ostream &os, const gubg::Style &style);

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
        static const Color fuchsia;
        static const Color cyan;
        static const Color yellow;
        static const Color olive;
        static const Color teal;
        static const Color purple;
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
        template <int Min, int Max>
        static Color shade(double sh)
        {
            if (sh < Min)
                return Color::red;
            if (sh > Max)
                return Color::green;
            //For symmetric color shades, we use black as the "middle" value to make it clear when the middle is hit
            if (Max == -Min)
            {
                const double middle = 0.5*(Max+Min);
                const double half = 0.5*(Max-Min);
                if (sh < middle)
                    return Color((middle-sh)/half, 0, 0);
                return Color(0, (sh-middle)/half, 0);
            }
            //Else, we just create a smooth range from red to green
            const double green = (sh-Min)/(Max-Min);
            return Color(1.0-green, green, 0);
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
