#include "gubg/tree/tree.hpp"
#include <string>
#include <iostream>
using namespace std;
#define L(m) cout<<m<<endl

namespace 
{
    typedef gubg::Tree<string> Tree;
}

int main()
{
    Tree::Hub root;
    root.push_back(string("abc"));
    root.push_back(string("def"));
    {
        Tree::Hub emptyHub;
        root.push_back(std::move(emptyHub));
    }
    root.push_back(string("ghi"));
    {
        Tree::Hub subHub;
        subHub.push_back(string("ABC"));
        subHub.push_back(string("DEF"));
        root.push_back(std::move(subHub));
    }
    root.push_back(string("jkl"));
    root.push_back(string("mno"));

    for (Tree::Hub::iterator it = root.begin(); it != root.end(); ++it)
    {
        L("\nNew iteration");
        L(it.path().size());
        L(it.isLeaf());
        L(it.isHub());
        if (it.isLeaf())
            L(it.leaf());
    }
    return 0;
}
