#include "gubg/point/point.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

int main()
{
    Point<> p(0.0, 1.0);
    cout << p << endl;
    TwoPoint<> tp(0, 1, 2, 3);
    cout << tp << endl;
    return 0;
}
