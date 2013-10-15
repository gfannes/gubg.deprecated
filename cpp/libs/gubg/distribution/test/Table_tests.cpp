#include "gubg/distribution/Table.hpp"
#include "gubg/mss.hpp"
using namespace gubg::distribution;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    MSS_BEGIN(int, main);
    vector<double> probs = {1.0, 2.0, 3.0};
    Table<double> table(probs);
    for (auto i = 0; i < 10; ++i)
    {
        L(table.generate());
    }

    table.setProbs(vector<double>{1.0, 1.0, 98.0});
    for (auto i = 0; i < 10; ++i)
    {
        L(table.generate());
    }
    MSS_END();
}
#include "gubg/log/end.hpp"
