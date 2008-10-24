#include "imageData.hpp"

#define fnBeans "/home/gfannes/data/raw/Beans"

int main(int argc, char *argv[])
{
    ImageData id(fnBeans);
    id.load(176);

    id.learn(0.3, 0.1);
    return 0;
}
