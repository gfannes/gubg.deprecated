#ifndef gubg_point_point_hpp
#define gubg_point_point_hpp

#include <ostream>

namespace gubg
{
    template <typename T = double>
        struct Point
        {
            Point(T xx, T yy):
                x(xx), y(yy){}
            T x;
            T y;
        };
}
template <typename T>
std::ostream &operator<<(std::ostream &os, const gubg::Point<T> &p)
{
    return os << "(" << p.x << ", " << p.y << ")";
}

namespace gubg
{
    template <typename T = double>
        struct TwoPoint
        {
            typedef Point<T> PointT;

            TwoPoint(T x0, T y0, T x1, T y1):
                p0(x0, y0), p1(x1, y1){}
            static TwoPoint centered(T x, T y, T w, T h)
            {
                w *= 0.5;
                h *= 0.5;
                return TwoPoint(x-w, y-h, x+w, y+h);
            }
            static TwoPoint centered(const PointT &center, T w, T h)
            {
                return TwoPoint::centered(center.x, center.y, w, h);
            }
            static TwoPoint expanded(TwoPoint source, T factor)
            {
                return TwoPoint::centered(source.centerX, source.centerY, factor*source.width, factor*source.height);
            }
            PointT p0;
            PointT p1;

            //L2 distance
            T length() const {return sqrt(pow(p0.x-p1.x, 2) + pow(p0.y-p1.y, 2));}
            T width() const {return abs(moveX());}
            T height() const {return abs(moveY());}
            T centerX() const {return 0.5*(p0.x+p1.x);}
            T centerY() const {return 0.5*(p0.y+p1.y);}
            //The distance to move across a certain axis to go from p0 to p1
            T moveX() const {return p1.x-p0.x;}
            T moveY() const {return p1.y-p0.y;}
            bool isInside(const Point<> &p) const
            {
                //Check the X-axis
                if (p0.x <= p1.x)
                {
                    if (p.x < p0.x || p1.x < p.x)
                        return false;
                } else
                {
                    if (p.x < p1.x || p0.x < p.x)
                        return false;
                }
                //Check the Y-axis
                if (p0.y <= p1.y)
                {
                    if (p.y < p0.y || p1.y < p.y)
                        return false;
                } else
                {
                    if (p.y < p1.y || p0.y < p.y)
                        return false;
                }
                //All axis match => p is inside the rectangle
                return true;
            }
        };
}
template <typename T>
std::ostream &operator<<(std::ostream &os, const gubg::TwoPoint<T> &tp)
{
    return os << "(" << tp.p0 << ", " << tp.p1 << ")";
}

#endif
