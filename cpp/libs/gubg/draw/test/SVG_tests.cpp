#include "gubg/draw/SVG.hpp"
#include "gubg/mss.hpp"
#include <iostream>
#include <fstream>
using gubg::draw::Circle;
using gubg::draw::Rectangle;
using gubg::draw::Line;
using gubg::draw::PiecewiseLinear;
using namespace gubg::draw::svg;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    const size_t Width = 800;
    const size_t Height = 600;
    typedef gubg::draw::RGB<unsigned char> RGB;
}

int main()
{
    MSS_BEGIN(int, main);
    SVG svg(Width, Height);
    {
        Group g;
        g.style().setStroke(RGB::Green).setWidth(5);
        for (auto i = 0; i < 20; ++i)
        {
            g.add(Circle<float>(40*i, 30*i, 20), std::move(Style().setFill(RGB(10*i, 0, 0))));
        }
        svg.add(std::move(g));
    }
    svg.add(Circle<double>(450, 60, 70), std::move(Style().setFill(RGB::Blue)));
    svg.add(Rectangle<double>(400, 150, 30, 40));
    svg.add(Line<double>(0, 0, Width-1, Height-1), std::move(Style().setStroke(RGB::Green)));
    svg.add(PiecewiseLinear<double>(vector<double>({0.0, 100.0, 200.0}), vector<double>({500.0, 200.0, 500.0})), std::move(Style().setStroke(RGB::Green)));

    cout << svg;
    ofstream fo("test.svg");
    fo << svg;
    MSS_END();
}
#include "gubg/log/end.hpp"
