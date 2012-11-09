#ifndef HEADER_gubg_visualization_color_hpp_ALREADY_INCLUDED
#define HEADER_gubg_visualization_color_hpp_ALREADY_INCLUDED

#include <iostream>

using namespace std;

class Color
{
public:
    Color():
        mValid(true),
        red(0),
        green(0),
        blue(0),
        alpha(1){};
    Color(double r, double g, double b, double a):
        mValid(true),
        red(r),
        green(g),
        blue(b),
        alpha(a){};
    Color(double grayScale):
        mValid(true),
        red(grayScale),
        green(grayScale),
        blue(grayScale),
        alpha(1.0){};
    bool valid(){return mValid;}
    void setValid(bool valid){mValid = valid;};
    void print(){cout << "red = " << red << ", green  = " << green << ", blue = " << blue << "" << endl;}
    static Color Invalid()
        {
            Color inv(0,0,0,0);
            inv.setValid(false);
            return inv;
        }
    static Color Black()
        {
            Color black(0,0,0,1);
            return black;
        }
    static Color White()
        {
            Color white(1,1,1,1);
            return white;
        }
    static Color Red()
        {
            Color red(1,0,0,1);
            return red;
        }
    static Color Green()
        {
            Color green(0,1,0,1);
            return green;
        }
    static Color Blue()
        {
            Color blue(0,0,1,1);
            return blue;
        }
    double red;
    double green;
    double blue;
    double alpha;
private:
    bool mValid;
};

#endif
