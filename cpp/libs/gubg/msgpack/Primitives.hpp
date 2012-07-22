#ifndef gubg_msgpack_Primitives_hpp
#define gubg_msgpack_Primitives_hpp

namespace gubg
{
    namespace msgpack
    {
        enum class Primitive
        {
            PosFix, UInt8, UInt16, UInt32, NegFix, Int8, Int16, Int32,
            Nil, Boolean, FixArray, 
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
    }
}

#endif
