#ifndef gubg_msgpack_Read_hpp
#define gubg_msgpack_Read_hpp

#include "gubg/msgpack/Primitives.hpp"
#include "gubg/msgpack/Codes.hpp"

namespace gubg
{
    namespace msgpack
    {
        template <typename String>
            class Range
            {
                public:
                    Range(const String &str):
                        it_(str.begin()), end_(str.end()){}

                    ReturnCode read(Type &type)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(it_ != end_, RangeIsEmpty);
                        int ch = *(it_++);
                        if (ch & 0x80)
                        {
                            if (ch & 0x40)
                            {
                                if (ch & 0x20)
                                {
                                    type.primitive = Primitive::NegFix;
                                    type.group = Group::Integer;
                                    type.nr = (ch &0x1f);
                                }
                                else
                                {
                                    //0xc* or 0xd* marker
                                    switch (ch)
                                    {
                                        case 0xc0:
                                            //nil
                                            break;
                                        case 0xc2:
                                            //false
                                            break;
                                        case 0xc3:
                                            //true
                                            break;
                                        case 0xca:
                                            //float
                                            break;
                                        case 0xcb:
                                            //double
                                            break;
                                        case 0xcc:
                                            //uint8
                                            break;
                                        case 0xcd:
                                            //uint16
                                            break;
                                        case 0xce:
                                            //uint32
                                            break;
                                        case 0xcf:
                                            //uint64
                                            break;
                                        case 0xd0:
                                            //int8
                                            break;
                                        case 0xd1:
                                            //int16
                                            break;
                                        case 0xd2:
                                            //int32
                                            break;
                                        case 0xd3:
                                            //int64
                                            break;
                                        case 0xda:
                                            //raw16
                                            break;
                                        case 0xdb:
                                            //raw32
                                            break;
                                        case 0xdc:
                                            //array16
                                            break;
                                        case 0xdd:
                                            //array32
                                            break;
                                        case 0xde:
                                            //map16
                                            break;
                                        case 0xdf:
                                            //map32
                                            break;
                                        default:
                                            MSS_L(UnknownType);
                                            break;
                                    }
                                }
                            }
                            else
                            {
                                if (ch & 0x20)
                                {
                                    //FixRaw
                                }
                                else
                                {
                                    if (ch & 0x10)
                                    {
                                        //FixArray
                                    }
                                    else
                                    {
                                        //FixMap
                                    }
                                }
                            }
                        }
                        else
                        {
                            type.primitive = Primitive::PosFix;
                            type.group = Group::Integer;
                            type.nr = (ch &0x7f);
                        }
                        MSS_END();
                    }

                private:
                    typedef typename String::const_iterator const_iterator;
                    const_iterator it_;
                    const const_iterator end_;
            };

        template <typename String, typename Integer>
            ReturnCode read(Integer &v, const String &str, SInteger_tag)
            {
                MSS_BEGIN(ReturnCode);
                Range<String> range(str);
                Type type;
                MSS(range.read(type));
                switch (type.primitive)
                {
                    case Primitive::PosFix:
                        v = type.nr;
                        break;
                    case Primitive::NegFix:
                        v = type.nr-32;
                        break;
                    default:
                        MSS_L(ReadError);
                        break;
                }
                MSS_END();
            }
        template <typename String>
            ReturnCode read(int &v, const String &str) { return read(v, str, SInteger_tag()); }
    }
}

#endif
