#ifndef gubg_draw_Color_hpp
#define gubg_draw_Color_hpp

#include <sstream>
#include <iomanip>

namespace gubg
{
    namespace draw
    {
        template <typename T> struct traits {};
        template <> struct traits<unsigned char> {static const unsigned char DefaultMaxValue = 0xff;};
        template <> struct traits<unsigned int>  {static const unsigned int  DefaultMaxValue = 0xffffffff;};
        template <> struct traits<int>           {static const int           DefaultMaxValue = 0x7fffffff;};
        template <> struct traits<unsigned long> {static const unsigned long DefaultMaxValue = 0xffffffff;};
        template <> struct traits<long>          {static const long          DefaultMaxValue = 0x7fffffff;};

        template <typename T, T MaxValue_ = traits<T>::DefaultMaxValue>
            struct RGB
            {
                static const T MaxValue = MaxValue_;
                T r;
                T g;
                T b;
                RGB(){}
                RGB(T rr, T gg, T bb):r(rr), g(gg), b(bb){}
                template <typename C>
                    RGB(const C &c):r(c.r), g(c.g), b(c.b){}

                static const RGB Black;
                static const RGB White;
                static const RGB Red;
                static const RGB Green;
                static const RGB Blue;
            };
        template <typename T, T MV>
            const RGB<T, MV> RGB<T, MV>::Black(0, 0, 0);
        template <typename T, T MV>
            const RGB<T, MV> RGB<T, MV>::White(MV, MV, MV);
        template <typename T, T MV>
            const RGB<T, MV> RGB<T, MV>::Red(MV, 0, 0);
        template <typename T, T MV>
            const RGB<T, MV> RGB<T, MV>::Green(0, MV, 0);
        template <typename T, T MV>
            const RGB<T, MV> RGB<T, MV>::Blue(0, 0, MV);
        template <typename T, T MaxValue_ = traits<T>::DefaultMaxValue>
            struct RGBA
            {
                static const T MaxValue = MaxValue_;
                T r;
                T g;
                T b;
                T a;
                RGBA(){}
                RGBA(T rr, T gg, T bb, T aa):r(rr), g(gg), b(bb), a(aa){}
                RGBA(const RGB<T, MaxValue> &c):r(c.r), g(c.g), b(c.b), a(MaxValue){}
                template <typename C>
                    RGBA(const C &c):r(c.r), g(c.g), b(c.b), a(c.a){}
            };

        template <typename Color>
            std::string to_html(const Color &c)
            {
                std::ostringstream oss;
                oss << '#' <<
                    std::hex << std::setw(2) << std::setfill('0') << (int)c.r <<
                    std::hex << std::setw(2) << std::setfill('0') << (int)c.g <<
                    std::hex << std::setw(2) << std::setfill('0') << (int)c.b;
                return oss.str();
            }
    }
}

#endif
