#include "vectorMask.hpp"
#include "matrixMask.hpp"
#include "vector.hpp"
#include "image.hpp"
#include "datavisu.hpp"

using namespace gubg;
using namespace gubg::Vector;

int main(int argc, char *argv[])
{
  vector<int> vec,it,et;
  set(vec,1,2,3,4,5,6,7,8,9,10);
  VectorMask vm(3,5);
  vm.filter(it, et, vec);

  cout << "Vec " << vec << endl;
  cout << "Int " << it << endl;
  cout << "Ext " << et << endl;

  MatrixMask mm(20, vm);
  Image im("CherriesBad_000.bmp");
  mm.filter(it, et, im.data());
  cout << "Img " << im.data() << endl;
  cout << "Int " << it << endl;
  cout << "Ext " << et << endl;

  MatrixMask mm2;
  mm2.learn(im.data(), 100);
  mm2.filter(it, et, im.data());
  cout << "Img " << im.data() << endl;
  cout << "Int " << it << endl;
  cout << "Ext " << et << endl;
  mm2.print();

  cout << "" << im << "" << endl;

  DataVisu dv(800,400);

  KernelD<int> kint, kext;
  kint.learn(it);
  kext.learn(et);
  cout << "width for int = " << kint.getWidth() << "" << endl;
  cout << "width for ext = " << kext.getWidth() << "" << endl;

  vector<double> xy,xz,yy,zz;
  Vector::setEqual(xy,300,0,256);
  Vector::setEqual(xz,300,0,256);

  for (int i=0;i<xy.size();++i)
    yy.push_back(kint.density(xy[i]));
  dv.addLines(xy,yy);
  cout << "added yy" << endl;

  for (int i=0;i<xz.size();++i)
    zz.push_back(kext.density(xz[i]));
  dv.addLines(xz,zz);
  cout << "added zz" << endl;

  dv.show();

  return 0;
}
