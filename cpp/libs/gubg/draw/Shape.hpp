#ifndef gubg_draw_Shape_hpp
#define gubg_draw_Shape_hpp

#include <vector>

namespace gubg
{
    namespace draw
    {
        template <typename T>
            struct Circle
            {
                T x;
                T y;
                T r;
                Circle(T xx, T yy, T rr):
                    x(xx), y(yy), r(rr){}
            };
        template <typename T>
            struct Rectangle
            {
                T x;
                T y;
                T dx;
                T dy;
                Rectangle(T xx, T yy, T ddxx, T ddyy):
                    x(xx), y(yy), dx(ddxx), dy(ddyy){}
            };
        template <typename T>
            struct Line
            {
                T x1;
                T y1;
                T x2;
                T y2;
                Line(T xx1, T yy1, T xx2, T yy2):
                    x1(xx1), y1(yy1), x2(xx2), y2(yy2){}
            };
        template <typename T>
            struct Point
            {
                T x;
                T y;
                Point(){}
                Point(T xx, T yy):
                    x(xx), y(yy){}
                template <typename P>
                    Point(const P &p):
                        x(p.x), y(p.y){}
                template <typename P>
                    Point &operator=(const P &p){x = p.x; y = p.y; return *this;}
            };
        template <typename T>
            struct PiecewiseLinear
            {
                typedef std::vector<Point<T> > Points;
                Points points;
                template <typename Points_>
                    PiecewiseLinear(const Points_ &pts):
                        points(pts.size())
                {
                    auto dst = points.begin();
                    for (const auto &p: pts)
                        *dst++ = p;
                }
                template <typename Xs, typename Ys>
                    PiecewiseLinear(const Xs &xs, const Ys &ys):
                        points(xs.size())
                {
                    auto dst = points.begin();
                    auto y = ys.begin();
                    for (const auto &x: xs)
                    {
                        auto &point = *dst++;
                        point.x = x;
                        point.y = *y++;
                    }
                }
            };
    }
}

#endif
