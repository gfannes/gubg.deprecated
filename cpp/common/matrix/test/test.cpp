#include "matrix.hpp"
#include "vector.hpp"

using namespace Vector;

int main()
{
  Matrix<double> m(10,10);
  vector<double> vec;
  setIncremental(vec,100,0.0,1.0);
  m.setElements(vec);
  cout << "Values\n" << m << "" << endl;

  Matrix<bool> mask(3,3);
  vector<bool> maskValues;
  set(maskValues,false,true,false,true,true,true,false,true,false);
  mask.setElements(maskValues);
  cout << "Mask\n" << mask << "" << endl;
  m.match(vec,mask,3,5);
  cout << "" << vec << "" << endl;
  return 0;
}
