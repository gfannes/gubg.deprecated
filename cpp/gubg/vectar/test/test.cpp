#include <iostream>

#include "vectar.hpp"
#include "function.hpp"
#include "vector.hpp"

using namespace std;
using namespace gubg;

class FunctionH: public Function<double,0,1>
  {
  public:
    bool computeOutput()
    {
      this->mOutput = (*this->mpInput)[0] * (*this->mpInput)[1];
      return true;
    }
    virtual bool clone(Function<double,0,1> *&res) const
      {
        return res = new FunctionH(*this);
      }
  };

int main()
{
  FunctionH function;
  MeanVectar<FunctionH> vectar(&function,0,1,2,0.3,4,5);
  vector<double> res;

  vectar.generate(res);

  cout << "" << res << "" << endl;
  return 0;
}
