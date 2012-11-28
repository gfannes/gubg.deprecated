#include "da/build/Configuration.hpp"
using namespace da;
using namespace gubg::file;

Configuration::Configuration()
{
    compiler.includePaths.push_back(File("../"));
    compiler.includePaths.push_back(File("/home/gfannes/gubg/cpp/libs"));
}
