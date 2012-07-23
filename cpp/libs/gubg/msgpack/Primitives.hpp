#ifndef gubg_msgpack_Primitives_hpp
#define gubg_msgpack_Primitives_hpp

namespace gubg
{
    namespace msgpack
    {
        enum class Primitive
        {
            PosFix, UInt8, UInt16, UInt32, NegFix, Int8, Int16, Int32,
            Nil, Boolean,
            FixArray, Array16, Array32,
            FixRaw, Raw16, Raw32,
        };
        enum class Group
        {
            Integer, Nil, Boolean, Array, Raw,
        };

        struct Type
        {
            Primitive primitive;
            Group group;
        };

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
    }
}

#endif
