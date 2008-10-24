#include <iostream>

#include "serialize.hpp"

using namespace std;

namespace Serialize
{
    vector< FuncptrAllocator > allocators;
    vector< FuncptrSerialize > serializers;
    vector< FuncptrUnserialize > unserializers;

    bool getAllocator(FuncptrAllocator &allocator, int typeID)
    {
        if (typeID < 0 || typeID >= allocators.size())
        {
            cerr << "TypeID is out of range: " << typeID << " and length is " << allocators.size() << endl;
            return false;
        }
        allocator = allocators[typeID];
        return true;
    };
    bool getSerialize(FuncptrSerialize &serialize, int typeID)
    {
        if (typeID < 0 || typeID >= serializers.size())
        {
            cerr << "TypeID is out of range: " << typeID << endl;
            return false;
        }
        serialize = serializers[typeID];
        return true;
    };
    bool getUnserialize(FuncptrUnserialize &unserialize, int typeID)
    {
        if (typeID < 0 || typeID >= unserializers.size())
        {
            cerr << "TypeID is out of range: " << typeID << endl;
            return false;
        }
        unserialize = unserializers[typeID];
        return true;
    };

}
