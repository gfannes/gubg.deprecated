#ifndef HEADER_gubg_msgpack_Primitives_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Primitives_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Codes.hpp"
#include "gubg/cstdint.hpp"

namespace gubg
{
	namespace msgpack
	{
		//We need a bool type to get rid of ambiguity with long
		class Bool
		{
			public:
				Bool():b_(false){}
				Bool(bool b):b_(b){}
				Bool(const Bool &rhs):b_(rhs.b_){}
				Bool &operator=(bool b) {b_ = b; return *this;}
				Bool &operator=(const Bool &rhs) {b_ = rhs.b_; return *this;}
				operator bool () const {return b_;}
				bool operator==(const Bool &rhs) const {return b_ == rhs.b_;}

			private:
				bool b_;
		};

		enum class Primitive: unsigned char
		{
			Unknown,//0
			PosFix, UInt8, UInt16, UInt32, UInt64,//1-5
			NegFix, Int8, Int16, Int32, Int64,//6-10
			Nil, True, False,//11-13
			Float, Double,//14-15
			FixArray, Array16, Array32,//16-18
			FixRaw, Raw16, Raw32,//19-21
			FixMap, Map16, Map32,//22-24
		};
		enum class Group: unsigned char
		{
			Unknown,//0
			Integer, Nil, Boolean, Float, Array, Raw, Map//1-7
		};
		enum class Width: unsigned char
		{
			Unknown = 255,
			Zero = 0, One = 1, Four = 4, Five = 5, Seven = 7, Eight = 8, Sixteen = 16, Thirtytwo = 32, Sixtyfour = 64,
		};

		struct Type
		{
			Primitive primitive;
			Group group;
			Width width;
			//Will contain the size encoded in this type itself (the type part is masked, so only the size bits remain)
			//* 0/1 for booleans
			//* the size for small array/maps
			//* the int value for posints
			unsigned char nr;

			Type(): primitive(Primitive::Unknown), group(Group::Unknown), width(Width::Zero), nr(0){}
			ReturnCode read(const std::uint8_t);
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
		L_MSGPACK_DEFINE_TAG(UInt64);
		L_MSGPACK_DEFINE_TAG(NegFix);
		L_MSGPACK_DEFINE_TAG(Int8);
		L_MSGPACK_DEFINE_TAG(Int16);
		L_MSGPACK_DEFINE_TAG(Int32);
		L_MSGPACK_DEFINE_TAG(Nil);
		L_MSGPACK_DEFINE_TAG(True);
		L_MSGPACK_DEFINE_TAG(False);
		L_MSGPACK_DEFINE_TAG(FixArray);
		L_MSGPACK_DEFINE_TAG(Array16);
		L_MSGPACK_DEFINE_TAG(Array32);
		L_MSGPACK_DEFINE_TAG(FixRaw);
		L_MSGPACK_DEFINE_TAG(Raw16);
		L_MSGPACK_DEFINE_TAG(Raw32);
		L_MSGPACK_DEFINE_TAG(FixMap);
		L_MSGPACK_DEFINE_TAG(Map16);
		L_MSGPACK_DEFINE_TAG(Map32);

		struct ArrayTL_tag {};//Array type and length
		struct MapTL_tag {};//Array type and length

		//Group tags
		struct Integer_tag {};
		struct SInteger_tag {};
		struct UInteger_tag {};
	}
}

#endif
