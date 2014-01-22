#include "gubg/Cartouche.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Testing.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);

    string content;
    file::read(content, file::File(__FILE__));

//!@#lua
//print("You can use !@# here without problems")
//print("This is still part of the cartouche")
 //The leading space stops the cartouche

    auto cartouches = Cartouche::parse(content);

    for (auto &ct: cartouches)
        cout << ct.tag << endl << ct.content << endl;
    return 0;
}
#include "gubg/log/end.hpp"
