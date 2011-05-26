#include "logging/Primitives.hpp"

#include <iostream>

using namespace gubg;

COutPrimitive::~COutPrimitive()
{
}

void COutPrimitive::write(const std::string &str)
{
    std::cout << str;
}

FilePrimitive::FilePrimitive(const std::string &fileName):
    _outputFile(fileName.c_str(), std::ios_base::out | std::ios_base::trunc)// | std::ios_base::ate)
{
}

FilePrimitive::~FilePrimitive()
{
    _outputFile.close();
}

void FilePrimitive::write(const std::string &str)
{
    _outputFile << str;
    _outputFile.flush();
}

#ifdef UNIT_TEST
int main()
{
    COutPrimitive cp;
    cp.write("asdfasdf");

    return 0;
}
#endif
