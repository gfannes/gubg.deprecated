#ifndef serialize_h
#define serialize_h

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

namespace Serialize
{
    typedef int TypeID;

    // Pointer to a function, used to store the allocate methods
    typedef void* (*FuncptrAllocator)();
    // Pointer to a function, used to store the serialize methods
    typedef bool (*FuncptrSerialize)(void*,ostream&);
    // Pointer to a function, used to store the unserialize methods
    typedef bool (*FuncptrUnserialize)(void*,istream&);
    extern vector< FuncptrAllocator > allocators;
    extern vector< FuncptrSerialize > serializers;
    extern vector< FuncptrUnserialize > unserializers;
    
    bool getAllocator(FuncptrAllocator &allocator, int typeID);
    bool getSerialize(FuncptrSerialize &serialize, int typeID);
    bool getUnserialize(FuncptrUnserialize &unserialize, int typeID);

    // The default template struct that expects the class T to have defined the methods:
    //  * static TypeID id4Serialize()
    //  * static T* allocate4Serialize()
    //  * bool serialize4Serialize(ostream &output)
    //  * bool unserialize4Serialize(istream &input)
    template< typename T >
    struct Methods
    {
        static const TypeID typeID;
        static T* allocate()
            {
                return T::allocate4Serialize();
            };
        static bool serialize(T &p,ostream &output)
            {
                return p.serialize4Serialize(output);
            };
        static bool unserialize(T &p,istream &input)
            {
                return p.unserialize4Serialize(input);
            };
        static bool serializeUsingPtr(T *p,ostream &output)
            {
                return p->serialize4Serialize(output);
            };
        static bool unserializeUsingPtr(T *p,istream &input)
            {
                return p->unserialize4Serialize(input);
            };
    };

    // Run this macro in the Serialize namespace to attach and id to a type
    // that defines the correct member methods
#define SetType(type,typeidh)                                        \
    template<>                                                        \
    struct Methods<type>                                                \
    {                                                                \
        static const TypeID typeID = typeidh;                        \
        static type* allocate()                                        \
        {                                                        \
            return type::allocate4Serialize();                        \
        };                                                        \
        static bool serialize(type &p,ostream &output)                \
        {                                                        \
            return p.serialize4Serialize(output);                \
        };                                                        \
        static bool unserialize(type &p,istream &input)                \
        {                                                        \
            return p.unserialize4Serialize(input);                \
        };                                                        \
        static bool serializeUsingPtr(type *p,ostream &output)        \
        {                                                        \
            return p->serialize4Serialize(output);                \
        };                                                        \
        static bool unserializeUsingPtr(type *p,istream &input)        \
        {                                                        \
            return p->unserialize4Serialize(input);                \
        };                                                        \
    };                                                                \
    
    // Run this macro in the Serialize namespace to attach an id and functions to a type
    // that does not define the correct member methods
#define SetTypeAndFunctions(type,typeidh,allocF,serializeF,unserializeF)        \
    template< >                                                                \
    struct Methods<type>                                                \
    {                                                                        \
        static const TypeID typeID = typeidh;                                \
        static type* allocate()                                                \
        {                                                                \
            return allocF();                                                \
        };                                                                \
        static bool serialize(type &p,ostream &output)                        \
        {                                                                \
            return serializeF(p,output);                                \
            };                                                                \
        static bool unserialize(type &p,istream &input)                        \
        {                                                                \
            return unserializeF(p,input);                                \
        };                                                                \
        static bool serializeUsingPtr(type *p,ostream &output)                        \
        {                                                                \
            return serializeF(*p,output);                                \
            };                                                                \
        static bool unserializeUsingPtr(type *p,istream &input)                        \
        {                                                                \
            return unserializeF(*p,input);                                \
        };                                                                \
    };                                                                        \

    template< typename T >
    inline TypeID typeID()
    {
        return Methods<T>::typeID;
    }

    template< typename T >
    inline bool allocate(T *&p)
    {
        p=Methods<T>::allocate();
        return (p ? true : false);
    }

    template< typename T >
    inline bool serialize(T &t,ostream &output)
    {
        return Methods<T>::serialize(t,output);
    }
    template< typename T >
    inline bool serializeWithType(T &t,ostream &output)
    {
        static int typeIDH;
        typeIDH = Methods<T>::typeID;
        return serialize(typeIDH,output) && serialize(t,output);
    }

    template< typename T >
    inline bool unserialize(T &t,istream &input)
    {
        return Methods<T>::unserialize(t,input);
    }
    template< typename T >
    inline bool unserializeWithType(T &t,istream &input)
    {
        TypeID typeID;
        if (!unserialize(typeID,input))
        {
            cerr << "Could not read the type info" << endl;
            return false;
        }
        if (typeID != Methods<T>::typeID)
        {
            cerr << "Typeids are different" << endl;
            return false;
        }
        return unserialize(t,input);
    }
    template< typename T >
    inline bool unserializeWithType(T *&p,TypeID &typeID,istream &input)
    {
        if (!unserialize(typeID,input))
            return false;
        cout << "typeID = " << typeID << endl;
        FuncptrAllocator allocator;
        FuncptrUnserialize unserializer;
        if (getAllocator(allocator,typeID) && getUnserialize(unserializer,typeID))
        {
            cout << "functions are ok " << allocator << " " << unserializer << endl;
            return (p=(T*)allocator()) && unserializer((void*)p,input);
        }
        return false;
    }

    // Register functions for binary serialization for a specific type
    template< typename T >
    bool registerType()
    {
        TypeID typeID = Methods<T>::typeID;
        if (typeID>=allocators.size()) // allocators can be valid, e.g., if the string stuff is already set
            allocators.resize(typeID+1,NULL);
        if (typeID>=serializers.size()) // serialize and unserialize should always run equal
        {
            serializers.resize(typeID+1,NULL);
            unserializers.resize(typeID+1,NULL);
        }
        if (!allocators[typeID])
            allocators[typeID]=(FuncptrAllocator)Methods<T>::allocate;
        if (serializers[typeID])
            return false;        // This is already set
        serializers[typeID]=(FuncptrSerialize)Methods<T>::serializeUsingPtr;
        unserializers[typeID]=(FuncptrUnserialize)Methods<T>::unserializeUsingPtr;
        return true;
    };
};


// You have to define the type TypeID in here as an enum an fill it with the wanted ids
#include "typeIDs.hpp"

#endif
