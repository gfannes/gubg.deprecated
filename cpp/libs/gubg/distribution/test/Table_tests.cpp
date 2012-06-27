#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/distribution/Table.hpp"
#include "gubg/mss.hpp"
using namespace gubg::distribution;
using namespace std;

int main()
{
    MSS_BEGIN(int, main);
    vector<double> probs = {1.0, 2.0, 3.0};
    Table<double> table(probs);
    for (auto i = 0; i < 10; ++i)
        LOG_M(table.generate());

    table.setProbs({1.0, 1.0, 98.0});
    for (auto i = 0; i < 10; ++i)
        LOG_M(table.generate());
    MSS_END();
}
