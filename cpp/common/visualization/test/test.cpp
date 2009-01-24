#include <iostream>
#include <vector>

#include "visualization.hpp"
#include "random.hpp"

using namespace std;

typedef struct
{
    Circle *circle;
    double x;
    double y;
} StuffT;

void func(double &r,double &g,double &b,double x,double y)
{
    r = 0.5+0.5*sin(x);
    g = 0.5+0.5*sin(y)*sin(x/(abs(y+x)+1));
    b = 0.5+0.5*sin(x*y);
}

class VisualizationH: public Visualization
{
public:
    VisualizationH(int xW, int yW):Visualization(xW, yW){};
    vector<StuffT> circles;
private:
    virtual bool dynamics()
        {
            int i;
            StuffT *stuff;
            double noise;

            for (i=0;i<circles.size();i++)
            {
                noise=sin(mTotalElapsedTime/1000000.0);
                stuff=&circles[i];
                stuff->circle->setCentre(stuff->x+mRandom.drawUniform(-noise,noise),stuff->y+mRandom.drawUniform(-noise,noise));
                stuff->circle->setRadius(1.5*noise*noise);
            }
            // cout << "Total elapsed time: " << mTotalElapsedTime << "" << endl; // do(0)
            return mTotalElapsedTime < 3141529;
        }
    Random mRandom;
};

int main(int argc, char *argv[])
{
    int basis;
    Line *line;
    Circle *circle;
    VisualizationH visu(1200,600);
    int i;
    int nr=36;
    double x,y;
    StuffT stuff;

    basis=visu.createBasis(600,300,30,30);

    Surface *surface = new Surface(&func,-5,-5,10,10);
    visu.addDrawable(basis,surface);

    for (i=0;i<nr;i++)
    {
        x=10*cos(2*3.1415926/nr*i);
        y=10*sin(2*3.1415926/nr*i);
        line=new Line(0,0,x,y);
        visu.addDrawable(basis,line);
        circle=new Circle(x,y,2,3,Color::Black(),Color::Green());
        stuff.circle=circle;
        stuff.x=x;
        stuff.y=y;
        visu.circles.push_back(stuff);
        visu.addDrawable(basis,circle);
    }
    visu.start();
    return 0;
}
