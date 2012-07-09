#ifndef gubg_draw_SVG_hpp
#define gubg_draw_SVG_hpp

#include "gubg/draw/Shape.hpp"
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
            //Interface that is used to stream elements and items to an xml tag
            struct IStream
            {
                typedef std::unique_ptr<IStream> Ptr;
                virtual ~IStream(){}
                virtual void stream(xml::builder::Tag &tag) const = 0;
            };
            //PolyWraps that add IStream to basic shapes
            template <typename T, typename Tag>
                void stream_pw(const Circle<T> &c, Tag &tag)
                {
                    tag.tag("circle").attr("cx", c.x).attr("cy", c.y).attr("r", c.r);
                }
            template <typename T, typename Tag>
                void stream_pw(const Rectangle<T> &r, Tag &tag)
                {
                    tag.tag("rect").attr("x", r.x).attr("y", r.y).attr("width", r.width).attr("height", r.height);
                }
            //A group
            class Group: public IStream
            {
                public:
                    Group(){}
                    Group(Group &&group):
                        shapes_(std::move(group.shapes_)){}

                    template <typename Shape>
                        Group &add(const Shape &shape)
                        {
                            shapes_.push_back(IStream::Ptr(new ShapePW<Shape>(shape)));
                            return *this;
                        }
                    Group &add(Group &&group)
                    {
                        shapes_.push_back(IStream::Ptr(new Group(std::move(group))));
                        return *this;
                    }
                    //IStream API
                    virtual ~Group(){}
                    virtual void stream(xml::builder::Tag &) const;

                private:
                    template <typename Shape>
                        struct ShapePW: IStream, Shape
                    {
                        ShapePW(const Shape &shape):Shape(shape){}
                        virtual ~ShapePW(){}
                        virtual void stream(xml::builder::Tag &tag) const {return stream_pw(static_cast<const Shape&>(*this), tag);}
                    };
                    typedef std::vector<IStream::Ptr> Shapes;
                    Shapes shapes_;
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
