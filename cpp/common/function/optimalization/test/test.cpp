#include <iostream>
#include <cmath>

#include "optimalization.hpp"
#include "block.hpp"
#include "datavisu.hpp"

using namespace std;

class Sine:public Function<double, 0, 0>
{
public:
    Sine():
        mFactor(1.0){}
    virtual bool computeOutput()
        {
            if (this->mpInput)
            {
                mOutput = sin(*this->mpInput*mFactor);
                return true;
            }
            return false;
        }
    virtual bool clone(Function<double,0,0> *&res) const {};
    void setFactor(double factor)
        {
            mFactor = factor;
            cout << "factor = " << mFactor << "" << endl;
        }
private:
    double mFactor;
};

class BlockH: public Block<Optimizer<Sine> >
{
public:
    bool preYield()
        {
            values.resize(0);
        }
    bool yield(bool &changed, Sine &sine)
        {
            values.push_back(sine.output());
            cout << "" << sine.output() << "" << endl;
            sine.setFactor(index/20);
            changed = true;
            return true;
        }
    vector<double> values;
};

int main(int argc, char *argv[])
{
    Sine sine;
    double x=10,value;
    Optimizer<Sine> opt(200);
    BlockH b;
    cout << sine.output(x) << endl;
    opt.optimize(x,sine,&value,&b);
    cout << "input = " << x << " and function value = " << value << endl;

    cout << "" << b.values << "" << endl;

    DataVisu datavisu(1200,600);
    datavisu.addLines(b.values);
    datavisu.show();
    
    return 0;
}
