#include <vector>
#include <iostream>

#include "vector.hpp"

using namespace std;
using namespace gubg;
using namespace gubg::Vector;

int main()
{
  int v0=1234;
  cout << toString(v0) << endl;

  vector<int> v1a;
  vector<int> v1b;
  vector<int> v1c;
  vector<vector<int> > v2;
  vector<vector<vector<int> > > v3;
  vector<vector<vector<vector<int> > > > v4;
  set(v1a,1,2,3);
  set(v1b,10,20,30);
  set(v1c,100,200,300);
  v2.push_back(v1a);
  v2.push_back(v1b);
  v2.push_back(v1c);
  v3.push_back(v2);
  v3.push_back(v2);
  v4.push_back(v3);
  v4.push_back(v3);
  cout << Dimension<vector<int> >::dimension << endl;
  cout << toString(v1a) << endl;
  cout << toString(v1b) << endl;
  cout << toString(v1c) << endl;
  cout << toString(v2) << endl;
  cout << toString(v3) << endl;
  cout << "v4" << endl << toString(v4) << endl;

  vector<int> path;
  int *pi;
  initIter(path,v4);
  cout << toString(path) << endl;
  for (initIter(path,v4);proceedIter(path,v4,pi);)
    {
      cout << toString(path) << " => " << *pi << endl;
      getAtIter(path,v4,pi);
      cout << toString(path) << " => " << *pi << endl;
    }
  cout << toString(path) << endl;

  cout << endl << "for zero dim" << endl;
  for (initIter(path,v0);proceedIter(path,v0,pi);)
    {
      cout << toString(path) << " => " << *pi << endl;
      getAtIter(path,v0,pi);
      cout << toString(path) << " => " << *pi << endl;
    }
  cout << toString(path) << endl;
  return 0;
}
