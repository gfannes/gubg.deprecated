#include "convexRegion.hpp"
#include "image.hpp"
#include "matrixFunction.hpp"
#include "datavisu.hpp"

int main(int argc, char *argv[])
{
    typedef ConvexRegion::Point Point;
    vector<Point> points;
    Point p;
    p.set(-3, 2);
    points.push_back(p);
    p.set(1, 2);
    points.push_back(p);
    p.set(-1, 0);
    points.push_back(p);
    p.set(3, -2);
    points.push_back(p);
    p.set(2, 1);
    points.push_back(p);
    p.set(-1, 3);
    points.push_back(p);

    p.set(-1, 3);
    points.push_back(p);
    p.set(0, 0);
    points.push_back(p);
    p.set(0, 0);
    points.push_back(p);
    p.set(0, 2);
    points.push_back(p);

    ConvexRegion cr;
    cr.learn(points);
    cr.print();

    p.set(2, 2);
    cout << "isInternal = " << cr.isInternal(p) << "" << endl;

    Image im("CherriesBad_000.bmp");
    vector<vector<double> > imData;
    Cube::convert(imData, im.data());
    int m, M;
    Cube::findMinMax(m, M, im.data());
    double a, b;
    Math::computeLinTrans(a, b, m, 0.0, M, 1.0);
    MatrixFunction<double> mf(&imData, 0, 0, 1, 1, a, b);

    cr.learn(mf, 10, 400);
    cr.print();

    DataVisu dv(400, 400);
    dv.add2DFunction(0, 0, 1, 1, mf);
    for (int i=0; i<cr.nrSegments(); ++i)
    {
        Point start, end;
        cr.getSegment(start, end, i);
        dv.addLine(start.x, start.y, end.x, end.y);
    }
    dv.show();

    return 0;
}
