#include <iostream>
#include <vector>

#include "sequence.hpp"
#include "vector.hpp"

using namespace std;
using namespace gubg;

struct P: Sequence<double>::EachBlock
{
    bool yield(double v)
    {
        cout << "value = " << v << endl;
        return true;
    }
};

struct Sqr: Sequence<long>::CollectBlock<long>
{
    bool yield(long &res, long v)
    {
        res=v*v;
        return true;
    }
};

int main()
{
    // create inputs and targets
    vector<vector<double> > inputs;
    vector<double> targets;

    Sequence<double> sequence(-1.0,0.999,0.1);
    P p;
    sequence.each(p);

    vector<long> *pRes;
    Sequence<long> seq(10);
    Sqr sqr;
    pRes=seq.collect(sqr);
    cout << Vector::toString(*pRes) << endl;

    return 0;
}
