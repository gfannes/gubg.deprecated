#include <iostream>

#include "image.hpp"
#include "matrixFunction.hpp"
#include "vector.hpp"

using namespace std;

int main()
{
  Image image("CherriesBad_000.bmp");
  cout << image.toString() << endl;

  MatrixFunction<int> imageF(&image.data());
  vector<double> input;
  Vector::set(input,10.0,10.0);
  cout << "data = " << Vector::toString(image.data()) << "" << endl;
  cout << "input = " << Vector::toString(input) << ", output = " << imageF.output(input) << "" << endl;
  return 0;
}
