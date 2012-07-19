#ifndef gubg_draw_SVG_hpp
#define gubg_draw_SVG_hpp

#include "gubg/draw/Shape.hpp"
#include "gubg/draw/Style.hpp"
#include "gubg/xml/Builder.hpp"
#include <ostream>
#include <memory>
#include <vector>

namespace gubg
{
    namespace draw
    {
        namespace svg
        {
            class Group;
            class SVG;
        }
    }
}
std::ostream &operator<<(std::ostream &os, const gubg::draw::svg::SVG &svg);

namespace gubg
{
    namespace draw
    {
        namespace svg
        {
            typedef RGBA<unsigned char> Color;
            typedef gubg::draw::Style<Color, double> Style;

            void addStyle_(xml::builder::Tag &, const Style &);

            //Interface that is used to stream elements and items to an xml tag
            struct IStream
            {
                typedef std::unique_ptr<IStream> Ptr;
                virtual ~IStream(){}
                virtual void stream(xml::builder::Tag &tag) const = 0;
            };
            //PolyWraps that add IStream to basic shapes
            namespace pw
            {
                template <typename T, typename Tag>
                    void stream(const Circle<T> &c, Tag &tag)
                    {
                        tag.tag("circle").attr("cx", c.x).attr("cy", c.y).attr("r", c.r);
                    }
                template <typename T, typename Tag>
                    void stream(const Circle<T> &c, const Style &s, Tag &tag)
                    {
                        addStyle_(tag.tag("circle").attr("cx", c.x).attr("cy", c.y).attr("r", c.r), s);
                    }
                template <typename T, typename Tag>
                    void stream(const Rectangle<T> &r, Tag &tag)
                    {
                        tag.tag("rect").attr("x", r.x-0.5*r.dx).attr("y", r.y-0.5*r.dy).attr("width", r.dx).attr("height", r.dy);
                    }
                template <typename T, typename Tag>
                    void stream(const Rectangle<T> &r, const Style &s, Tag &tag)
                    {
                        addStyle_(tag.tag("rect").attr("x", r.x-0.5*r.dx).attr("y", r.y-0.5*r.dy).attr("width", r.dx).attr("height", r.dy), s);
                    }
                template <typename T, typename Tag>
                    void stream(const Line<T> &l, Tag &tag)
                    {
                        tag.tag("line").attr("x1", l.x1).attr("y1", l.y1).attr("x2", l.x2).attr("y2", l.y2);
                    }
                template <typename T, typename Tag>
                    void stream(const Line<T> &l, const Style &s, Tag &tag)
                    {
                        addStyle_(tag.tag("line").attr("x1", l.x1).attr("y1", l.y1).attr("x2", l.x2).attr("y2", l.y2), s);
                    }
                template <typename T>
                    std::string path_(const PiecewiseLinear<T> &pwl)
                    {
                        std::ostringstream path;
                        bool first = true;
                        for (auto p: pwl.points)
                        {
                            path << (first ? "M" : " L") << ' ' << p.x << ' ' << p.y;
                            first = false;
                        }
                        return path.str();
                    }
                template <typename T, typename Tag>
                    void stream(const PiecewiseLinear<T> &pwl, Tag &tag)
                    {
                        tag.tag("path").attr("d", path_(pwl));
                    }
                template <typename T, typename Tag>
                    void stream(const PiecewiseLinear<T> &pwl, const Style &s, Tag &tag)
                    {
                        addStyle_(tag.tag("path").attr("d", path_(pwl)), s);
                    }
            }
            //A group
            class Group: public IStream
            {
                public:
                    Group(){}
                    Group(Group &&group):
                        shapes_(std::move(group.shapes_)),
                        style_(std::move(group.style_)){}

                    template <typename Shape>
                        Group &add(const Shape &shape)
                        {
                            shapes_.push_back(IStream::Ptr(new ShapePW<Shape>(shape)));
                            return *this;
                        }
                    template <typename Shape>
                        Group &add(const Shape &shape, Style &&style)
                        {
                            shapes_.push_back(IStream::Ptr(new ShapeStylePW<Shape>(shape, std::move(style))));
                            return *this;
                        }
                    Group &add(Group &&group)
                    {
                        shapes_.push_back(IStream::Ptr(new Group(std::move(group))));
                        return *this;
                    }

                    Style &style(){return style_;}

                    //IStream API
                    virtual ~Group(){}
                    virtual void stream(xml::builder::Tag &) const;

                private:
                    template <typename Shape>
                        struct ShapePW
                        :IStream, Shape
                        {
                            ShapePW(const Shape &shape):Shape(shape){}
                            virtual ~ShapePW(){}
                            virtual void stream(xml::builder::Tag &tag) const {return pw::stream(static_cast<const Shape&>(*this), tag);}
                        };
                    template <typename Shape>
                        struct ShapeStylePW
                        :IStream, Shape, Style
                        {
                            ShapeStylePW(const Shape &shape, Style &&style):Shape(shape), Style(std::move(style)){}
                            virtual ~ShapeStylePW(){}
                            virtual void stream(xml::builder::Tag &tag) const {return pw::stream(static_cast<const Shape&>(*this), static_cast<const Style&>(*this), tag);}
                        };
                    typedef std::vector<IStream::Ptr> Shapes;
                    Shapes shapes_;
                    Style style_;
            };
            //An SVG is a group with size settings
            class SVG: public Group
            {
                public:
                    SVG(size_t width, size_t height);

                    size_t width() const {return width_;}
                    size_t height() const {return height_;}

                private:
                    const size_t width_;
                    const size_t height_;
            };
        }
    }
}

#endif
