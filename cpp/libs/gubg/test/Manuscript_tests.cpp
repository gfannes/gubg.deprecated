#include "gubg/Manuscript.hpp"
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

//!@#lua#@!.
//The previous line is _not_ a valid tag line, it contains extra characters after the stop sequence
//!@#lua#@!
//print("You can use !@# here without problems")
//print("This is still part of the paragraph")
 //The leading space stops the paragraph

    auto manuscript = Manuscript(content);

    for (auto &para: manuscript)
        cout << para.tag() << " " << para.lines().size() << endl;
    return 0;
}
#include "gubg/log/end.hpp"
