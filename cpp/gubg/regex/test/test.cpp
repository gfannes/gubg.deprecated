#include <iostream>

#include "regex.hpp"

using namespace std;

int main()
{
  Regex regex("Flower.+_\\d\\d\\d\\.bmp");
  string str("FlowerGood_123.bmp");
  cout << regex.match(str) << endl;
  return 0;
}
