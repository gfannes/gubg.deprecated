#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/bayesian/Dirichlet.hpp"
#include <iostream>
using namespace gubg::bayesian;
using namespace std;
#define L(m) cout<<m<<endl

int main()
{
    MSS_BEGIN(int, main);
    typedef Dirichlet<6, double, double> DieFamily;
    DieFamily dieFamily;
    auto dieFamilySave = dieFamily;
    LOG_M(dieFamily);
    dieFamily.observeDataPoint(1, 20.5);
    LOG_M(dieFamily);
    LOG_M(dieFamilySave);
    DieFamily::DataPoint die;
    for (auto i = 0; i < 10; ++i)
    {
        dieFamily.generate(die);
        LOG_M(die);
    }
    MSS_END();
}
