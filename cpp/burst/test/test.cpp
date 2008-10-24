#include <iostream>

#include "mlp.hpp"
#include "vector.hpp"
#include "visualization.hpp"

using namespace std;

class VisualizationH: public Visualization
{
public:
    VisualizationH(int xW, int yW):Visualization(xW,yW){};
private:
    virtual bool dynamics()
        {
            return mCurrentTick < 314152;
        }
};

int main(int argc, char *argv[])
{
    int basis;
    Line *line;
    Circle *circle;
    VisualizationH visu(1200,600);
    int i;
    int nr=360;
    double x,y;

    vector<int> nrPerLayer;
    Vector::set(nrPerLayer,4,5,2);
    vector<NeuronE> typePerLayer;
    Vector::set(typePerLayer,eNeuronTanh,eNeuronTanh,eNeuronTanh);
    typedef MLP<vector<double> > MLPT;
    MLPT mlp(2,nrPerLayer,typePerLayer);
    mlp.setStructure(2,nrPerLayer,typePerLayer);
    vector<double> input;
    vector<double> prevOutput;
    Vector::set(input,-2.0,1.0);
    MLPT::ParameterType *par;
    par=mlp.generateParameter();
    mlp.setParameter(*par);
    mlp.setInput(input);
    vector<double> diams;
    Vector::setEqual(diams,50,0.1,10.0);
    int j;
        
    basis=visu.createBasis(600,300,300,300);
    for (j=0;j<diams.size();j++)
    {
        for (i=0;i<nr+1;i++)
        {
            x=diams[j]*cos(2*3.1415926/nr*i);
            y=diams[j]*sin(2*3.1415926/nr*i);
            Vector::set(input,x,y);
            mlp.setInput(input);
            if (i>0)
            {
                line=new Line(prevOutput[0],prevOutput[1],mlp.output()[0],mlp.output()[1]);
                visu.addDrawable(basis,line);
            }
            prevOutput = mlp.output();
        }
    }
    visu.start();
    return 0;
}
