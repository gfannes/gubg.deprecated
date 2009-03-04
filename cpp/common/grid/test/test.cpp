#include <iostream>

#include "grid.hpp"
#include "vector.hpp"

using namespace std;
//using namespace Vector;

class BlockH: public Block<Grid>
{
public:
    bool yield(double x, double y)
        {
            cout << "x = " << x << ", y = " << y << "" << endl;
            return true;
        }
    bool yield(double &res, double x, double y)
        {
            res = x*y;
            return true;
        }
};

int main()
{
    Grid grid(0,1,2,3,4,5);
    BlockH block;
    vector<double> v;

    grid.each(block);
    grid.collect(v,block);

    cout << "" << v << "" << endl;

    return 0;
}
