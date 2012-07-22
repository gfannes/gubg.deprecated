#ifndef gubg_msgpack_Write_hpp
#define gubg_msgpack_Write_hpp

#include "gubg/msgpack/Codes.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include <vector>
#include <deque>
#include <list>

namespace gubg
{
	namespace msgpack
	{
		//Positive integers
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, PosFix_tag)
			{
				if (v < 0)
					return ReturnCode::OutOfRange;
				if (v > 0x7f)
					return ReturnCode::OutOfRange;
				buffer.push_back((char)(v & 0xff));
				return ReturnCode::OK;
			}
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, UInt8_tag)
			{
				if (v < 0)
					return ReturnCode::OutOfRange;
				if (v > 0xff)
					return ReturnCode::OutOfRange;
				buffer.push_back(0xcc);
				buffer.push_back((char)(v & 0xff));
				return ReturnCode::OK;
			}
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, UInt16_tag)
			{
				if (v < 0)
					return ReturnCode::OutOfRange;
				if (v > 0xffff)
					return ReturnCode::OutOfRange;
				buffer.push_back(0xcd);
				buffer.push_back((char)((v >> 8) & 0xff));
				buffer.push_back((char)(v & 0xff));
				return ReturnCode::OK;
			}
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, UInt32_tag)
			{
				if (v < 0)
					return ReturnCode::OutOfRange;
				if (v > 0xffffffff)
					return ReturnCode::OutOfRange;
				buffer.push_back(0xce);
				buffer.push_back((char)((v >> 24) & 0xff));
				buffer.push_back((char)((v >> 16) & 0xff));
				buffer.push_back((char)((v >> 8) & 0xff));
				buffer.push_back((char)(v & 0xff));
				return ReturnCode::OK;
			}
		//Negative integers
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, NegFix_tag)
			{
				if (v < -32)
					return ReturnCode::OutOfRange;
				if (v > -1)
					return ReturnCode::OutOfRange;
				buffer.push_back((char)(v & 0xff));
				return ReturnCode::OK;
			}
		//Signed integers
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, Int8_tag)
			{
				if (v < -128)
					return ReturnCode::OutOfRange;
				if (v > 127)
					return ReturnCode::OutOfRange;
				buffer.push_back(0xd0);
				buffer.push_back((char)(v & 0xff));
				return ReturnCode::OK;
			}
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, Int16_tag)
			{
				if (v < -32768)
					return ReturnCode::OutOfRange;
				if (v > 32766)
					return ReturnCode::OutOfRange;
				buffer.push_back(0xd1);
				buffer.push_back((char)((v >> 8) & 0xff));
				buffer.push_back((char)(v & 0xff));
				return ReturnCode::OK;
			}
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, Int32_tag)
			{
				//-2147483648 cannot be used directly because 2147483648 is too large, which blocks its negation
				if (v < -2147483648LL)
					return ReturnCode::OutOfRange;
				if (v > 2147483647)
					return ReturnCode::OutOfRange;
				buffer.push_back(0xd2);
				buffer.push_back((char)((v >> 24) & 0xff));
				buffer.push_back((char)((v >> 16) & 0xff));
				buffer.push_back((char)((v >> 8) & 0xff));
				buffer.push_back((char)(v & 0xff));
				return ReturnCode::OK;
			}
		struct integer_tag {};
		template <typename Buffer, typename Integer>
			ReturnCode write(Buffer &buffer, Integer v, integer_tag)
			{
				MSS_BEGIN(ReturnCode);
				if (v >= 0)
				{
					if (ReturnCode::OK == write(buffer, v, PosFix_tag()))
						return ReturnCode::OK;
					if (ReturnCode::OK == write(buffer, v, UInt8_tag()))
						return ReturnCode::OK;
					if (ReturnCode::OK == write(buffer, v, UInt16_tag()))
						return ReturnCode::OK;
					if (ReturnCode::OK == write(buffer, v, UInt32_tag()))
						return ReturnCode::OK;
					MSS_L(OutOfRange);
				}
				else
				{
					if (ReturnCode::OK == write(buffer, v, NegFix_tag()))
						return ReturnCode::OK;
					if (ReturnCode::OK == write(buffer, v, Int8_tag()))
						return ReturnCode::OK;
					if (ReturnCode::OK == write(buffer, v, Int16_tag()))
						return ReturnCode::OK;
					if (ReturnCode::OK == write(buffer, v, Int32_tag()))
						return ReturnCode::OK;
					MSS_L(OutOfRange);
				}
				MSS_END();
			}
		template <typename Buffer>
			ReturnCode write(Buffer &buffer, int v){return write(buffer, v, integer_tag());}
		template <typename Buffer>
			ReturnCode write(Buffer &buffer, unsigned int v){return write(buffer, v, integer_tag());}
		template <typename Buffer>
			ReturnCode write(Buffer &buffer, long int v){return write(buffer, v, integer_tag());}
		template <typename Buffer>
			ReturnCode write(Buffer &buffer, long long int v){return write(buffer, v, integer_tag());}

		//Nil
		template <typename Buffer>
			ReturnCode write(Buffer &buffer, Nil_tag)
			{
				buffer.push_back(0xc0);
				return ReturnCode::OK;
			}

        //Boolean
		template <typename Buffer>
			ReturnCode write(Buffer &buffer, bool v)
			{
				buffer.push_back(v ? 0xc3 : 0xc2);
				return ReturnCode::OK;
			}

        //Array
		template <typename Buffer, typename T>
	}
}

#endif
