//#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/draw/SVG.hpp"
#include "gubg/mss.hpp"
#include <iostream>
#include <fstream>
using namespace gubg::draw;
using namespace gubg::draw::svg;
using namespace std;

namespace 
{
    const size_t Width = 800;
    const size_t Height = 600;
}

int main()
{
    MSS_BEGIN(int, main);
    SVG svg(Width, Height);
    {
        Group g;
        g.style().setStroke(RGB<unsigned char>::Green).setWidth(5);
        for (auto i = 0; i < 20; ++i)
        {
            g.add(Circle<float>(40*i, 30*i, 20), std::move(svg::Style().setFill(RGB<unsigned char>(10*i, 0, 0))));
        }
        svg.add(std::move(g));
    }
    svg.add(Circle<double>(450, 60, 70), std::move(svg::Style().setFill(RGB<unsigned char>::Blue)));
    svg.add(Rectangle<double>(400, 150, 30, 40));
    svg.add(Line<double>(0, 0, Width-1, Height-1));
    svg.add(PiecewiseLinear<double>({0.0, 100.0, 200.0}, {500.0, 200.0, 500.0}));

    cout << svg;
    ofstream fo("test.svg");
    fo << svg;
    MSS_END();
}
