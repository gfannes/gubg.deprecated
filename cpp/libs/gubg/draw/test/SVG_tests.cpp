//#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/draw/SVG.hpp"
#include "gubg/mss.hpp"
#include <iostream>
using namespace gubg::draw;
using namespace gubg::draw::svg;
using namespace std;

int main()
{
    MSS_BEGIN(int, main);
    SVG svg(800, 500);
    svg.add(Circle<double>(50, 60, 70));
    svg.add(Rectangle<double>(1,2,3,4));
    {
        Group g;
        g.add(Circle<float>(100, 110, 20));
        svg.add(std::move(g));
    }
    cout << svg;
    MSS_END();
}
