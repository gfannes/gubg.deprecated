
#include "datavisu.hpp"
#include "vector.hpp"
#include "function.hpp"
#include "image.hpp"
#include "matrixFunction.hpp"

using namespace std;
using namespace Vector;

class FunctionH: public Function<double,0,1>
{
public:
    virtual bool computeOutput()
        {
            if (!this->mpInput || nrInputs()!=2)
                return false;
            this->mOutput = ((*this->mpInput)[0])*((*this->mpInput)[1]);
        }
};

int main(int argc, char *argv[])
{
    DataVisu datavisu(1200,600);
    vector<double> xs,ys;
    int i;
    
//     setEqual(xs,300,-5,5);
//     ys.resize(300);
//     for (i=0;i<xs.size();i++)
//         ys[i]=sin(xs[i]);
    
//     datavisu.addLines(xs,ys);


//     FunctionH function;
//     datavisu.add2DFunction(-4,-1,4,1,72,32,function);

    Image image("image.bmp");
    MatrixFunction<int> imageF(&image.data(),0,0,1,1,1.0/256,0,MatrixFunction<int>::eRepeatMatrix);
    datavisu.add2DFunction(-1,-1,2,2,imageF);

    datavisu.addRectangle(0,0,1,1);
    
    datavisu.show(true);

    sleep(1);
    datavisu.clear();
//     datavisu.add2DFunction(-1,-1,2,2,imageF);
    datavisu.addRectangle(0,0,1,1);
    datavisu.addRectangle(0.2,0.2,0.3,0.3);
    datavisu.redraw();
    cout << "After redraw" << endl;
    sleep(1);
    datavisu.clear();

    datavisu.finishThread();

    return 0;
};
