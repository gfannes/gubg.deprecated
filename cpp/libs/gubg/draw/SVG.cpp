#include "gubg/draw/SVG.hpp"
#include "gubg/xml/Builder.hpp"
#include <vector>
#include <sstream>
using namespace gubg::draw::svg;
using namespace gubg::xml::builder;
using namespace std;

void gubg::draw::svg::addStyle_(Tag &tag, const Style &style)
{
    if (style.fill)
        tag.attr("fill", to_html(*style.fill));
    else
        tag.attr("fill", "none");
    if (style.stroke)
        tag.attr("stroke", to_html(*style.stroke));
    if (style.width)
        tag.attr("stroke-width", *style.width);
}

void Group::stream(Tag &tag) const
{
    auto g = tag.tag("g");
    addStyle_(g, style_);
    for (auto &shape: shapes_)
        shape->stream(g);
}

SVG::SVG(size_t width, size_t height):
    width_(width), height_(height){}

ostream &operator<<(ostream &os, const SVG &svg)
{
    {
        Header(os).attr("version", "1.0").attr("encoding", "UTF-8");
    }
    {
        Comment(os) << "Created by gubg" << " (http://www.github.com/gfannes/gubg)";
    }
    Tag svgTag(os, "svg");
    svgTag.attr("width", svg.width());
    svgTag.attr("height", svg.height());
    svg.Group::stream(svgTag);
    return os;
}
