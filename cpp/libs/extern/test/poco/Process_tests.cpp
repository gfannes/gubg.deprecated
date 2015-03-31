#include "Poco/Process.h"
#include "Poco/Pipe.h"
#include "Poco/PipeStream.h"
#include "catch/catch.hpp"
#include <iostream>
using namespace Poco;
using namespace std;

TEST_CASE("Poco Process tests", "[poco]")
{
    Pipe out;
    auto h = Process::launch("git", Process::Args{"status"}, 0, &out, 0);
    PipeInputStream pip(out);
    REQUIRE(pip.good());
    string line;
    while (getline(pip, line))
        cout << " >>> " << line << endl;
}
