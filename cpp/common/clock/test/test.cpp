#include <iostream>

#include "timestamp.hpp"
#include "clock.hpp"

using namespace std;

int main()
{
    TimeStamp ts(12,34000000);

    ts.breakDown(false);
    cout << "" << ts << "" << endl;

    Clock::local().now(ts);
    ts.breakDown(true);
    cout << "" << ts << "" << endl;

    Clock::local().now(ts);
    ts.breakDown(false);
    cout << "" << ts << "" << endl;

    cout << Clock::local() << endl;

    for (int i=0;i<10;++i)
    {
        Clock clock;
        int sum=0;
        for (int j=0;j<i*100000000;++j)
            sum += j;
        cout << "Iteration " << i << " took " << clock.elapse() << "" << endl;
    }

    return 0;
}
