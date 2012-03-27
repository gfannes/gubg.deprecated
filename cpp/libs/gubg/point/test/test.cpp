#include "gubg/point/point.hpp"
#include "gubg/testing/Testing.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(Point);
        Point<> p(0.0, 1.0);
        cout << p << endl;
    }
    {
        TEST_TAG(TwoPoint);
        TwoPoint<> tp(0, 1, 2, 3);
        cout << tp << endl;
        tp = TwoPoint<>::expanded(tp, 2.0);
        cout << tp << endl;
    }
    return 0;
}
