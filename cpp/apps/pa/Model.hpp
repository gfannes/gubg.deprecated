#ifndef HEADER_pa_Model_hpp_ALREADY_INCLUDED
#define HEADER_pa_Model_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>
#include <map>

namespace pa
{
    struct Node
    {
        typedef std::string Name;
        typedef double Value;
        typedef double Fraction;
        typedef std::vector<Node> Childs;
        typedef std::string Id;
        typedef std::map<std::string, std::string> Attributes;

        Name desc;
        Value value;
        Fraction fraction;
        Childs childs;
        Id id;
        Attributes attributes;

        Node():value(), fraction(){}
    };

    inline Node &model()
    {
        static Node root;
        return root;
    }
}

#endif
