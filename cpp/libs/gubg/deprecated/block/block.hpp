#ifndef HEADER_gubg_deprecated_block_block_hpp_ALREADY_INCLUDED
#define HEADER_gubg_deprecated_block_block_hpp_ALREADY_INCLUDED

// This is a generic block class that can be used. You can derive from this, but it is not necessary, since all block users should use templates

#include <iostream>

using namespace std;

template <typename ContextT>
class Block
{
public:
    typedef ContextT ContextType;

    Block():
        context(NULL){}

    bool preYield(){return true;}
    bool yield(){cerr << "You should not use the yield from the base block" << endl;return false;}
    bool postYield(){return true;}
    template <typename T>
    bool postYield(T t){return true;}

    long index;
    ContextType *context;
};

#endif
