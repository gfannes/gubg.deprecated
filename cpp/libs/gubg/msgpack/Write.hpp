#ifndef HEADER_gubg_msgpack_Write_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Write_hpp_ALREADY_INCLUDED

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
        template <typename Buffer, typename Integer>
            ReturnCode write(Buffer &buffer, Integer v, Integer_tag)
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
            ReturnCode write(Buffer &buffer, int v){return write(buffer, v, Integer_tag());}
        template <typename Buffer>
            ReturnCode write(Buffer &buffer, unsigned int v){return write(buffer, v, Integer_tag());}
        template <typename Buffer>
            ReturnCode write(Buffer &buffer, long int v){return write(buffer, v, Integer_tag());}
        template <typename Buffer>
            ReturnCode write(Buffer &buffer, long long int v){return write(buffer, v, Integer_tag());}

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

        template <typename Buffer>
            ReturnCode write(Buffer &buffer, const size_t nr, ArrayTL_tag)
            {
                MSS_BEGIN(ReturnCode);
                if (nr <= 15)
                    buffer.push_back(0x90 | nr);
                else if (nr <= 65535)
                {
                    buffer.push_back(0xdc);
                    buffer.push_back((char)((nr >> 8) & 0xff));
                    buffer.push_back((char)(nr & 0xff));
                }
                else
                {
                    buffer.push_back(0xdd);
                    buffer.push_back((char)((nr >> 24) & 0xff));
                    buffer.push_back((char)((nr >> 16) & 0xff));
                    buffer.push_back((char)((nr >> 8) & 0xff));
                    buffer.push_back((char)(nr & 0xff));
                }
                MSS_END();
            }
        template <typename Buffer, typename Container>
            ReturnCode write(Buffer &buffer, const Container &container, FixArray_tag)
            {
                MSS_BEGIN(ReturnCode);
                auto s = container.size();
                MSS_Q(s <= 15, TooLarge);
                buffer.push_back(0x90 | s);
                for (const auto &v: container)
                    MSS(write(buffer, v));
                MSS_END();
            }
        template <typename Buffer, typename Container>
            ReturnCode write(Buffer &buffer, const Container &container, Array16_tag)
            {
                MSS_BEGIN(ReturnCode);
                auto s = container.size();
                MSS_Q(s <= 65535, TooLarge);
                buffer.push_back(0xdc);
                buffer.push_back((char)((s >> 8) & 0xff));
                buffer.push_back((char)(s & 0xff));
                for (const auto &v: container)
                    MSS(write(buffer, v));
                MSS_END();
            }
        template <typename Buffer, typename Container>
            ReturnCode write(Buffer &buffer, const Container &container, Array32_tag)
            {
                MSS_BEGIN(ReturnCode);
                auto s = container.size();
                MSS_Q(s <= 4294967295, TooLarge);
                buffer.push_back(0xdd);
                buffer.push_back((char)((s >> 24) & 0xff));
                buffer.push_back((char)((s >> 16) & 0xff));
                buffer.push_back((char)((s >> 8) & 0xff));
                buffer.push_back((char)(s & 0xff));
                for (const auto &v: container)
                    MSS(write(buffer, v));
                MSS_END();
            }
        template <typename Buffer, typename T>
            ReturnCode write(Buffer &buffer, const std::vector<T> &ary)
            {
                if (ReturnCode::OK == write(buffer, ary, FixArray_tag()))
                    return ReturnCode::OK;
                if (ReturnCode::OK == write(buffer, ary, Array16_tag()))
                    return ReturnCode::OK;
                if (ReturnCode::OK == write(buffer, ary, Array32_tag()))
                    return ReturnCode::OK;
                return ReturnCode::TooLarge;
            }
        template <typename Buffer, typename T>
            ReturnCode write(Buffer &buffer, const std::deque<T> &ary)
            {
                if (ReturnCode::OK == write(buffer, ary, FixArray_tag()))
                    return ReturnCode::OK;
                if (ReturnCode::OK == write(buffer, ary, Array16_tag()))
                    return ReturnCode::OK;
                if (ReturnCode::OK == write(buffer, ary, Array32_tag()))
                    return ReturnCode::OK;
                return ReturnCode::TooLarge;
            }
        template <typename Buffer, typename T>
            ReturnCode write(Buffer &buffer, const std::list<T> &ary)
            {
                //Suboptimal implementation
                if (ReturnCode::OK == write(buffer, ary, FixArray_tag()))
                    return ReturnCode::OK;
                if (ReturnCode::OK == write(buffer, ary, Array16_tag()))
                    return ReturnCode::OK;
                if (ReturnCode::OK == write(buffer, ary, Array32_tag()))
                    return ReturnCode::OK;
                return ReturnCode::TooLarge;
            }

        //Raw
        template <typename Buffer, typename String>
            ReturnCode write(Buffer &buffer, const String &str, FixRaw_tag)
            {
                MSS_BEGIN(ReturnCode);
                auto s = str.size();
                MSS_Q(s <= 31, TooLarge);
                buffer.push_back(0xa0 | s);
                buffer.append(str);
                MSS_END();
            }
        template <typename Buffer, typename String>
            ReturnCode write(Buffer &buffer, const String &str, Raw16_tag)
            {
                MSS_BEGIN(ReturnCode);
                auto s = str.size();
                MSS_Q(s <= 65535, TooLarge);
                buffer.push_back(0xda);
                buffer.push_back((char)((s >> 8) & 0xff));
                buffer.push_back((char)(s & 0xff));
                buffer.append(str);
                MSS_END();
            }
        template <typename Buffer, typename String>
            ReturnCode write(Buffer &buffer, const String &str, Raw32_tag)
            {
                MSS_BEGIN(ReturnCode);
                auto s = str.size();
                MSS_Q(s <= 4294967295, TooLarge);
                buffer.push_back(0xdb);
                buffer.push_back((char)((s >> 24) & 0xff));
                buffer.push_back((char)((s >> 16) & 0xff));
                buffer.push_back((char)((s >> 8) & 0xff));
                buffer.push_back((char)(s & 0xff));
                buffer.append(str);
                MSS_END();
            }
        template <typename Buffer>
            ReturnCode write(Buffer &buffer, const std::string &str)
            {
                if (ReturnCode::OK == write(buffer, str, FixRaw_tag()))
                    return ReturnCode::OK;
                if (ReturnCode::OK == write(buffer, str, Raw16_tag()))
                    return ReturnCode::OK;
                if (ReturnCode::OK == write(buffer, str, Raw32_tag()))
                    return ReturnCode::OK;
                return ReturnCode::TooLarge;
            }
    }
}

#endif
