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
    struct Dir: gubg::tree::managed::Node<raw::Dir>
    {
        Dir(){}
        static Dir create(const string &path)
        {
            //dir will hold our return value, a Dir all the way to the root
            Dir dir;
            MSS_BEGIN_J();
            //Expand path into pathE as an absolute path
            string pathE;
            MSS_J(gubg::file::basic::expandPath(pathE, path));
            //We split pathE into tokens and use these to construct a single-path tree with a root
            {
                auto tokens = gubg::parse::tokenize(pathE, "/");
                //Check that the first part is empty, which should be for an absolute path
                MSS_J(!tokens.empty());
                MSS_J(tokens[0] == "");
                //Create the root
                tokens.pop_front();
                dir.setRoot_(new raw::Dir());
                dir->name = "";
                //Gradually construct the tree from the tokens parts
                //dir will serve as the parent, and gradually move along
                for (; !tokens.empty(); tokens.pop_front())
                {
                    auto &token = tokens.front();
                    Dir node(new raw::Dir(), dir);
                    node->name = token;
                    dir.add(node);
                    dir = node;
                }
            }
            MSS_FAIL();
            return dir;
        }
        typedef gubg::tree::managed::Node<raw::Dir> Base;
        Dir(RawPtr node, Base &parent):
            Base(node, parent){}
    };
}

int main()
{
    managed::Dir root;
    {
        LOG("\nTesting managed");
        auto home = managed::Dir::create("./");
        home.getRoot(root);
    }
    LOG("After scope");

    for (auto it = root->begin(gubg::tree::ByData); it != root->end(gubg::tree::ByData); ++it)
    {
        LOG("Data: " << it->name);
    }
#if 0
    for (auto it = root.begin(gubg::tree::ByNode); it != root.end(gubg::tree::ByNode); ++it)
    {
        LOG("Data: " << it->name);
    }
#endif
    return 0;
}
