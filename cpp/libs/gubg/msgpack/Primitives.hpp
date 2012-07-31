#ifndef gubg_msgpack_Primitives_hpp
#define gubg_msgpack_Primitives_hpp

#include "gubg/msgpack/Codes.hpp"

namespace gubg
{
    namespace msgpack
    {
        typedef unsigned char ubyte;

        enum class Primitive: unsigned char
        {
            Unknown,
            PosFix, UInt8, UInt16, UInt32, UInt64,
            NegFix, Int8, Int16, Int32, Int64,
            Nil, Boolean,
            Float, Double,
            FixArray, Array16, Array32,
            FixRaw, Raw16, Raw32,
            FixMap, Map16, Map32,
        };
        enum class Group: unsigned char
        {
            Unknown,
            Integer, Nil, Boolean, Float, Array, Raw, Map
        };
        enum class Width: unsigned char
        {
            Unknown,
            Zero = 0, One = 1, Four = 4, Five = 5, Seven = 7, Eight = 8, Sixteen = 16, Thirtytwo = 32, Sixtyfour = 64,
        };

        struct Type
        {
            Primitive primitive;
            Group group;
            Width width;
            unsigned char nr;

            Type(): primitive(Primitive::Unknown), group(Group::Unknown), width(Width::Zero), nr(0){}
            ReturnCode read(const ubyte);
            void clear();
            ReturnCode valid() const;
        };

        //Primitive tags
#define L_MSGPACK_DEFINE_TAG(name) \
        struct name ## _tag {static const Primitive primitive = Primitive::name;}
        L_MSGPACK_DEFINE_TAG(PosFix);
        L_MSGPACK_DEFINE_TAG(UInt8);
        L_MSGPACK_DEFINE_TAG(UInt16);
        L_MSGPACK_DEFINE_TAG(UInt32);
        L_MSGPACK_DEFINE_TAG(NegFix);
        L_MSGPACK_DEFINE_TAG(Int8);
        L_MSGPACK_DEFINE_TAG(Int16);
        L_MSGPACK_DEFINE_TAG(Int32);
        L_MSGPACK_DEFINE_TAG(Nil);
        L_MSGPACK_DEFINE_TAG(Boolean);
        L_MSGPACK_DEFINE_TAG(FixArray);
        L_MSGPACK_DEFINE_TAG(Array16);
        L_MSGPACK_DEFINE_TAG(Array32);
        L_MSGPACK_DEFINE_TAG(FixRaw);
        L_MSGPACK_DEFINE_TAG(Raw16);
        L_MSGPACK_DEFINE_TAG(Raw32);

        //Group tags
        struct Integer_tag {};
        struct SInteger_tag {};
        struct UInteger_tag {};
    }
}

#endif
