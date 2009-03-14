#include <iostream>
#include <vector>

#include "reporting.hpp"
#include "vector.hpp"

using namespace std;
using namespace gubg;

int main()
{
  vector<int> reals, preds;
  Vector::set(reals, 1, 2, 3, 1, 2, 3);
  Vector::set(preds, 1, 2, 3, 2, 2, 3);
  vector<vector<int> > confusion;
  Reporting::confusionCounts(confusion, reals, preds);
  cout << "" << confusion << "" << endl;

  vector<int> labels;
  vector<double> dpreds;
  vector<double> xs, ys;
//     Vector::set(labels, 1, 0, 1, 1, 0, 1, 1, 0, 1);
//     Vector::set(dpreds, 0.1, 0.2, 0.3, 0.11, 0.21, 0.13, 0.21, 0.22, 0.32);
  Vector::set(labels, 0, 0, 0, 0, 1, 0, 1, 1, 1);
  Vector::set(dpreds, 0.1, 0.2, 0.3, 0.41, 0.42, 0.43, 0.51, 0.52, 0.53);
  Reporting::rocCurve(xs, ys, labels, dpreds);
  cout << "xs = " << xs << "" << endl;
  cout << "ys = " << ys << "" << endl;

  cout << "auc = " << Reporting::areaUnderROC(labels, dpreds) << "" << endl;
  return 0;
}
