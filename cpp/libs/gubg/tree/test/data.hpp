#ifndef HEADER_gubg_tree_test_data_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_test_data_hpp_ALREADY_INCLUDED

#include "gubg/tree/raw.hpp"
#include "gubg/file/basic.hpp"
#include "gubg/parse/basic.hpp"
#include <iostream>
#define LOG(msg) std::cout << msg << std::endl

//This is the data we want to store for each node of the tree
struct Data
{
    ~Data(){LOG("Data::dtor " << name);}
    std::string name;
};

//We create the composite and leaf node types that will store Data, _without_ memory management
namespace raw
{
    using namespace std;
    struct Dir: gubg::tree::raw::Node<Data>
    {
        string path()
        {
            ostringstream oss;
            Nodes nodes = pathFromRoot();
            for (auto node = nodes.begin(); node != nodes.end(); ++node)
            {
                if (node != nodes.begin())
                    oss << "/";
                oss << (*node)->name;
            }
            return oss.str();
        }
    };
    struct Reg: gubg::tree::raw::Node<Data>
    {
        string filename()
        {
            ostringstream oss;
            Nodes nodes = pathFromRoot();
            for (auto node = nodes.begin(); node != nodes.end(); ++node)
            {
                if (node != nodes.begin())
                    oss << "/";
                oss << (*node)->name;
            }
            return oss.str();
        }
    };
}

#endif
