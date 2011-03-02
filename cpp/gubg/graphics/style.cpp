#include "style.hpp"

namespace gubg
{
    const Color Color::black(0.0, 0.0, 0.0);
    const Color Color::white(1.0, 1.0, 1.0);
    const Color Color::red(1.0, 0.0, 0.0);
    const Color Color::green(0.0, 1.0, 0.0);
    const Color Color::blue(0.0, 0.0, 1.0);
    const Color Color::purple(1.0, 0.0, 1.0);
    const Color Color::cyan(0.0, 1.0, 1.0);
    const Color Color::yellow(1.0, 1.0, 0.0);
    const Color Color::gray(0.5, 0.5, 0.5);
    const Color Color::coolRed(0.33, 0.07, 0.20);
    const Color Color::darkRed(0.25, 0.06, 0.15);
    const Color Color::coolGreen(0.20, 0.33, 0.07);
    const Color Color::darkGreen(0.15, 0.25, 0.06);
    const Color Color::coolBlue(0.07, 0.20, 0.33);
    const Color Color::darkBlue(0.06, 0.15, 0.25);

    const double InvalidValue = -2.0;
    const Color Color::invalid;

    Color::Color():
        r(InvalidValue), g(InvalidValue), b(InvalidValue){}

    bool Color::isValid() const
    {
        return InvalidValue != r;
    }
}
std::ostream &operator<<(std::ostream &os, const gubg::Color &color)
{
        return os << "#(" << color.r << ", " << color.g << ", " << color.b << ")";
}
std::ostream &operator<<(std::ostream &os, const gubg::Style &style)
{
    if (style.hasFill())
        os << "Fill color: " << style.fill_ << std::endl;
    if (style.hasStroke())
        os << "Stroke color: " << style.stroke_ << ", stroke width: " << style.width_ << std::endl;
    return os;
}
