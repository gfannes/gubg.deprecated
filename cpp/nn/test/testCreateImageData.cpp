#include <iostream>
#include <vector>
#include <fstream>

#include "imageData.hpp"

using namespace std;

#define dirRaw "/home/gfannes/data/raw"


int main(int argc, char *argv[])
{
  vector<ImageData> infos;
  {
    ImageData info("Beans", "BeansGood", "BeansBad");
    infos.push_back(info);
  }
  Dir dir(dirRaw);
  for (int i=0; i<infos.size(); ++i)
    {
      ImageData &info = infos[i];
      info.create();
    }
  return 0;
}
