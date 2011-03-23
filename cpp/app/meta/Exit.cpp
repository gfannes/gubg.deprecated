#include "Exit.hpp"
#include <iostream>
#include <cstdlib>
using namespace std;

void meta::finalize(const string &msg)
{
    cout << msg << endl;
    exit(0);
}
