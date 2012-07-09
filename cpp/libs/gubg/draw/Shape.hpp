#ifndef gubg_draw_Shape_hpp
#define gubg_draw_Shape_hpp

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
            T width;
            T height;
            Rectangle(T xx, T yy, T ww, T hh):
                x(xx), y(yy), width(ww), height(hh){}
        };
    }
}

#endif
