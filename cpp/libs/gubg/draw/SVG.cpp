#include "gubg/draw/SVG.hpp"
#include "gubg/xml/Builder.hpp"
#include <vector>
using namespace gubg::draw::svg;
using namespace gubg::xml::builder;
using namespace std;

void Group::stream(Tag &tag) const
{
    auto g = tag.tag("g");
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
