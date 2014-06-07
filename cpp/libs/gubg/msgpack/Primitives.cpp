#include "gubg/msgpack/Primitives.hpp"

#define GUBG_MODULE "msgpack_Primitives"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace msgpack
    {
        ReturnCode Type::read(const uint8_t b)
        {
            MSS_BEGIN(ReturnCode);
            clear();
            if (b & 0x80)
            {
                //1??? ????
                if (b & 0x40)
                {
                    //11?? ????
                    if (b & 0x20)
                    {
                        //111? ????
                        primitive = Primitive::NegFix;
                        group = Group::Integer;
                        width = Width::Five;
                    }
                    else
                    {
                        //110? ????
                        //0xc* or 0xd* marker
                        switch (b)
                        {
                            case 0xc0:
                                primitive = Primitive::Nil;
                                group = Group::Nil;
                                width = Width::Zero;
                                break;
                            case 0xc2:
                                primitive = Primitive::False;
                                group = Group::Boolean;
                                width = Width::One;
                                break;
                            case 0xc3:
                                primitive = Primitive::True;
                                group = Group::Boolean;
                                width = Width::One;
                                break;
                            case 0xca:
                                primitive = Primitive::Float;
                                group = Group::Float;
                                width = Width::Thirtytwo;
                                break;
                            case 0xcb:
                                primitive = Primitive::Double;
                                group = Group::Float;
                                width = Width::Sixtyfour;
                                break;
                            case 0xcc:
                                primitive = Primitive::UInt8;
                                group = Group::Integer;
                                width = Width::Eight;
                                break;
                            case 0xcd:
                                primitive = Primitive::UInt16;
                                group = Group::Integer;
                                width = Width::Sixteen;
                                break;
                            case 0xce:
                                primitive = Primitive::UInt32;
                                group = Group::Integer;
                                width = Width::Thirtytwo;
                                break;
                            case 0xcf:
                                primitive = Primitive::UInt64;
                                group = Group::Integer;
                                width = Width::Sixtyfour;
                                break;
                            case 0xd0:
                                primitive = Primitive::Int8;
                                group = Group::Integer;
                                width = Width::Eight;
                                break;
                            case 0xd1:
                                primitive = Primitive::Int16;
                                group = Group::Integer;
                                width = Width::Sixteen;
                                break;
                            case 0xd2:
                                primitive = Primitive::Int32;
                                group = Group::Integer;
                                width = Width::Thirtytwo;
                                break;
                            case 0xd3:
                                primitive = Primitive::Int64;
                                group = Group::Integer;
                                width = Width::Sixtyfour;
                                break;
                            case 0xda:
                                primitive = Primitive::Raw16;
                                group = Group::Raw;
                                width = Width::Sixteen;
                                break;
                            case 0xdb:
                                primitive = Primitive::Raw32;
                                group = Group::Raw;
                                width = Width::Thirtytwo;
                                break;
                            case 0xdc:
                                primitive = Primitive::Array16;
                                group = Group::Array;
                                width = Width::Sixteen;
                                break;
                            case 0xdd:
                                primitive = Primitive::Array32;
                                group = Group::Array;
                                width = Width::Thirtytwo;
                                break;
                            case 0xde:
                                primitive = Primitive::Map16;
                                group = Group::Map;
                                width = Width::Sixteen;
                                break;
                            case 0xdf:
                                primitive = Primitive::Map32;
                                group = Group::Map;
                                width = Width::Thirtytwo;
                                break;
                            default:
                                MSS_L(UnknownType);
                                break;
                        }
                    }
                }
                else
                {
                    //10?? ????
                    if (b & 0x20)
                    {
                        //101? ????
                        primitive = Primitive::FixRaw;
                        group = Group::Raw;
                        width = Width::Five;
                    }
                    else
                    {
                        //100? ????
                        if (b & 0x10)
                        {
                            //1001 ????
                            primitive = Primitive::FixArray;
                            group = Group::Array;
                            width = Width::Four;
                        }
                        else
                        {
                            //1000 ????
                            primitive = Primitive::FixMap;
                            group = Group::Map;
                            width = Width::Four;
                        }
                    }
                }
            }
            else
            {
                //0??? ????
                primitive = Primitive::PosFix;
                group = Group::Integer;
                width = Width::Seven;
            }
            MSS(valid());
            //Extract the length for those types that have the length encoded together with the type
            switch (width)
            {
                case Width::One:   nr = (b & 0x01); break;
                case Width::Four:  nr = (b & 0x0f); break;
                case Width::Five:  nr = (b & 0x1f); break;
                case Width::Seven: nr = (b & 0x7f); break;
            }
            MSS_END();
        }

        void Type::clear()
        {
            primitive = Primitive::Unknown;
            group = Group::Unknown;
            width = Width::Unknown;
            nr = 0;
        }

        ReturnCode Type::valid() const
        {
            MSS_BEGIN(ReturnCode);
            MSS_Q(primitive != Primitive::Unknown, UnknownPrimitive);
            MSS(group != Group::Unknown, UnknownGroup);
            MSS(width != Width::Unknown, UnknownWidth);
            MSS((nr & 0x80) == 0x00, InvalidNr);
            MSS_END();
        }
    }
}
#include "gubg/log/end.hpp"
