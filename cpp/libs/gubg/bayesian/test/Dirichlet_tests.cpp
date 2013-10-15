#include "gubg/bayesian/Dirichlet.hpp"
using namespace gubg::bayesian;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    MSS_BEGIN(int, main);
    typedef Dirichlet<6, double, double> DieFamily;
    DieFamily dieFamily;
    auto dieFamilySave = dieFamily;
    L(dieFamily);
    dieFamily.observeDataPoint(1, 20.5);
    L(dieFamily);
    L(dieFamilySave);
    DieFamily::DataPoint die;
    for (auto i = 0; i < 10; ++i)
    {
        dieFamily.generate(die);
        L(die);
    }
    MSS_END();
}
#include "gubg/log/end.hpp"
