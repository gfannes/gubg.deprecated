#include "test/data.hpp"
#include "gubg/tree/managed.hpp"
#include "gubg/file/basic.hpp"
#include "gubg/parse/basic.hpp"
#include <string>
#include <sstream>
#include <memory>
using namespace std;

//We create the composite and leaf node types that will store Data, _with_ memory management
namespace managed
{
    enum class ReturnCode {MSS_DEFAULT_CODES, CouldNotExpand, UnexpectedPath, CouldNotAddNode};
    struct Reg: gubg::tree::managed::Node<raw::Reg, raw::Dir>
    {
    };
    struct Dir: gubg::tree::managed::Node<raw::Dir, raw::Dir>
    {
        static ReturnCode create(Dir &dir, const string &path)
        {
            MSS_BEGIN(ReturnCode);
            //Expand path into pathE as an absolute path
            string pathE;
            MSS_T(gubg::file::basic::expandPath(pathE, path), CouldNotExpand);
            //We split pathE into tokens and use these to construct a single-path tree with a root
            auto tokens = gubg::parse::tokenize(pathE, "/");
            //Check that the first part is empty, which should be for an absolute path
            MSS_T(!tokens.empty(), UnexpectedPath);
            MSS_T(tokens[0] == "", UnexpectedPath);
            //Create the root
            dir = gubg::tree::managed::createRoot<Dir>(new raw::Dir);
            dir->name = "";
            tokens.pop_front();
            //Gradually construct the tree from the tokens parts
            //dir will serve as the parent, and gradually move along
            for (; !tokens.empty(); tokens.pop_front())
            {
                Dir node;
                MSS_T(dir.add_(node, new raw::Dir), CouldNotAddNode);
                node->name = tokens.front();
                dir = node;
            }
            MSS_END();
        }

        ReturnCode add(Reg &reg, const string &name)
        {
            MSS_BEGIN(ReturnCode);
            MSS_T(add_(reg, new raw::Reg), CouldNotAddNode);
            reg->name = name;
            MSS_END();
        }
    };
}

int main()
{
    managed::Dir dir;
    managed::Dir::create(dir, "./");
    LOG(dir->path());
    managed::Reg reg;
    dir.add(reg, "blabla");
    LOG(reg->filename());
    return 0;
}
