#include <vector>
#include <iostream>

#include "cube.hpp"
#include "vector.hpp"

using namespace std;

int main()
{
  vector<double> vec;
  vec.push_back(1.234);
  vec.push_back(1.235);
  vec.push_back(1.236);
  for (Cube::Iterator<double,1> iter(vec); iter.valid(); ++iter)
    cout << "" << *iter << "" << endl;

  double d = 123.321;
  for (Cube::Iterator<double,0> iter(d); iter.valid(); ++iter)
    cout << "" << *iter << "" << endl;

  vector<vector<double> > v2;
  v2.push_back(vec);
  v2.push_back(vec);
  vec.push_back(12.12);
  vector<double> v1;
  v2.push_back(v1);
  v2.push_back(vec);
  cout << "v2 = \n" << v2 << "" << endl;
  for (Cube::Iterator<double,2> iter(v2); iter.valid(); ++iter)
    {
      cout << "v2 element = " << *iter << "" << endl;
    }

  v2.resize(2);
  v2[1][1] = 23.0;
  vector<vector<double> > v22;
  Vector::set(vec, 1.0, 2.0, 3.0);
  v22.push_back(vec);
  v22.push_back(vec);
  v22[1][1] = 12.0;
  cout << "v2 = \n" << v2 << "" << endl;
  cout << "v22 = \n" << v22 << "" << endl;
  for (Cube::Iterator<double,2> iter(v2); iter.valid(); ++iter)
    {
      cout << "v2 element = " << *iter << "" << endl;
      iter.change(v22);
      cout << "v22 element = " << *iter << "" << endl;
      iter.change(v2);
    }

  return 0;
}
