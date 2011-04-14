#include "Exit.hpp"
#include <iostream>
#include <cstdlib>
using namespace std;

void meta::finalize(const string &msg, meta::Status status)
{
    int ret = 0;
    switch (status)
    {
        case meta::Status::Ok: break;
        case meta::Status::Error:
                               cout << "ERROR::";
                               ret = -1;
                               break;
    }
    cout << msg << endl;
    exit(ret);
}
