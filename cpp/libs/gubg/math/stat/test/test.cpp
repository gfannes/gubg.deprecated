#include <iostream>
#include <vector>

#include "gaussian.hpp"
#include "kernel.hpp"

#include "datavisu.hpp"

using namespace std;
using namespace gubg;

int main(int argc, char *argv[])
{
  GaussianD<double> gaussian;
  double x=0;
  cout << gaussian.logDensity(x) << endl;

  double d;
  gaussian.generate(d);
  cout << "" << d << "" << endl;

  KernelD<double> kernel;
  vector<double> data;
  Vector::set(data, 1.0,2.0,3.0);
  kernel.learn(data);
  kernel.setWidth(0.1);

  DataVisu dv(800,400);
  vector<double> xx,yy;
  Vector::setEqual(xx,300,0.0,4.0);
  for (int i=0;i<xx.size();++i)
    {
      yy.push_back(kernel.density(xx[i]));
    }
  dv.addLines(xx,yy);
  dv.show();
  return 0;
}
