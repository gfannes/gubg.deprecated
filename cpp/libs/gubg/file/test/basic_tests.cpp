#include "gubg/file/basic.hpp"
#include <iostream>
using namespace std;

#define LOG(msg) cout << msg << endl

int main()
{
    string pathE;
    gubg::file::basic::expandPath(pathE, "./");
    LOG(pathE);
    return 0;
}
