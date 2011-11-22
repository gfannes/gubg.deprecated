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
    enum class ReturnCode {MSS_DEFAULT_CODES, CouldNotExpand, CouldNotAddNode};
    struct Dir: gubg::tree::managed::Node<raw::Dir>
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
            MSS(!tokens.empty());
            MSS(tokens[0] == "");
            //Create the root
            dir = gubg::tree::managed::createRoot<Dir>(new raw::Dir);
            dir->name = "";
            tokens.pop_front();
            //Gradually construct the tree from the tokens parts
            //dir will serve as the parent, and gradually move along
            for (; !tokens.empty(); tokens.pop_front())
            {
                auto &token = tokens.front();
                LOG("token: " << token);
                Dir node;
                MSS_T(dir.add_(node, new raw::Dir), CouldNotAddNode);
                node->name = token;
                dir = node;
            }
            MSS_END();
        }
    };
}

int main()
{
    managed::Dir dir;
    managed::Dir::create(dir, "./");
    return 0;
}
