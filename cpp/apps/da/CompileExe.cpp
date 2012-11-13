#include "da/CompileExe.hpp"
#include "da/compile/Compiler.hpp"
using namespace da;
using namespace da::compile;
using namespace std;

CompileExe::CompileExe(const string &source):
    source_(source){}

ReturnCode CompileExe::execute(const Options &options)
{
    MSS_BEGIN(ReturnCode);
    Compiler compiler;
    gubg::file::File object(source_);
    object.setExtension("obj");
    MSS(compiler(source_, object));
    MSS_END();
}
