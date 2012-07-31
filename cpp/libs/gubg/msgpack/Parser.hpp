#ifndef gubg_msgpack_Parser_hpp
#define gubg_msgpack_Parser_hpp

#include "gubg/msgpack/Codes.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include <vector>

namespace gubg
{
    namespace msgpack
    {
        struct Element
        {
            Type type;
            size_t length;
            size_t ix;

            void clear() { type.clear(); }
        };
        typedef std::vector<Element> Path;

        template <typename ReceiverPolicy>
            class Parser: public ReceiverPolicy
        {
            public:
                ReturnCode process(ubyte b)
                {
                    MSS_BEGIN(ReturnCode, process);
                    LOG_M(STREAM((int)b));
                    if (mss::isOK(el_.type.valid()))
                    {
                        LOG_M("type is valid, " << STREAM(nrBytes_));
                        if (nrBytes_ > 0)
                            buffer_[--nrBytes_] = b;
                    }
                    else
                    {
                        LOG_M("type is not valid");
                        //We are ready to read a new object
                        MSS(el_.type.read(b));
                        LOG_M(STREAM((int)el_.type.width));
                        buffer_.fill(0x00);
                        switch (el_.type.width)
                        {
                            case Width::Four:
                            case Width::Five:
                            case Width::Seven:
                                buffer_[0] = el_.type.nr;
                                nrBytes_ = 0;
                                break;
                            case Width::Eight:
                                nrBytes_ = 1;
                                break;
                            case Width::Sixteen:
                                nrBytes_ = 2;
                                break;
                            case Width::Thirtytwo:
                                nrBytes_ = 4;
                                break;
                            case Width::Sixtyfour:
                                nrBytes_ = 8;
                                break;
                            default:
                                break;
                        }
                    }
                    if (nrBytes_ == 0)
                    {
                        LOG_M("all bytes are read: " << (int)buffer_[1] << ", " << (int)buffer_[0]);
                        switch (el_.type.group)
                        {
                            case Group::Array:
                                MSS(convertUInt_(el_.length, el_.type.width, buffer_));
                                el_.ix = 0;
                                ReceiverPolicy::open(el_, path_);
                                path_.push_back(el_);
                                el_.clear();
                                break;
                            case Group::Map:
                                MSS(convertUInt_(el_.length, el_.type.width, buffer_));
                                el_.ix = 0;
                                ReceiverPolicy::open(el_, path_);
                                path_.push_back(el_);
                                el_.clear();
                                break;
                            case Group::Integer:
                                switch (el_.type.primitive)
                                {
                                    case Primitive::PosFix:
                                    case Primitive::UInt8:
                                    case Primitive::UInt16:
                                    case Primitive::UInt32:
                                    case Primitive::UInt64:
                                        {
                                            unsigned long l;
                                            MSS(convertUInt_(l, el_.type.width, buffer_));
                                            ReceiverPolicy::add(l, path_);
                                            MSS_Q(proceed_());
                                        }
                                        break;
                                    case Primitive::NegFix:
                                    case Primitive::Int8:
                                    case Primitive::Int16:
                                    case Primitive::Int32:
                                    case Primitive::Int64:
                                        {
                                            long l;
                                            MSS(convertInt_(l, el_.type.width, buffer_));
                                            ReceiverPolicy::add(l, path_);
                                            MSS_Q(proceed_());
                                        }
                                        break;
                                }
                                break;
                        }
                    }
                    MSS_END();
                }

                void clear()
                {
                    path_.clear();
                    el_.type.clear();
                }

            private:
                typedef std::array<ubyte, 8> Buffer;

                ReturnCode proceed_()
                {
                    MSS_BEGIN(ReturnCode);
                    el_.clear();
                    MSS_Q(!path_.empty(), ParsingFinished);
                    auto &el = path_.back();
                    ++el.ix;
                    if (el.ix == el.length)
                    {
                        auto e = path_.back();
                        path_.pop_back();
                        ReceiverPolicy::close(e, path_);
                    }
                    MSS_END();
                }
                template <typename Size>
                    static ReturnCode convertUInt_(Size &nr, Width w, const Buffer &buffer)
                    {
                        MSS_BEGIN(ReturnCode);
                        switch (w)
                        {
                            case Width::Four:
                            case Width::Five:
                            case Width::Seven:
                            case Width::Eight:
                                nr = buffer[0];
                                break;
                            case Width::Sixteen:
                                MSS(sizeof(Size) >= 2, TooLarge);
                                nr = buffer[1];
                                nr <<= 8; nr |= buffer[0];
                                break;
                            case Width::Thirtytwo:
                                MSS(sizeof(Size) >= 4, TooLarge);
                                nr = buffer[3];
                                nr <<= 8; nr |= buffer[2];
                                nr <<= 8; nr |= buffer[1];
                                nr <<= 8; nr |= buffer[0];
                                break;
                            case Width::Sixtyfour:
                                MSS(sizeof(Size) >= 8, TooLarge);
                                nr = buffer[7];
                                nr <<= 8; nr |= buffer[6];
                                nr <<= 8; nr |= buffer[5];
                                nr <<= 8; nr |= buffer[4];
                                nr <<= 8; nr |= buffer[3];
                                nr <<= 8; nr |= buffer[2];
                                nr <<= 8; nr |= buffer[1];
                                nr <<= 8; nr |= buffer[0];
                                break;
                            default:
                                MSS_L(UnsupportedWidth);
                        }
                        MSS_END();
                    }
                template <typename Size>
                    static ReturnCode convertInt_(Size &nr, Width w, const Buffer &buffer)
                    {
                        MSS_BEGIN(ReturnCode);
                        switch (w)
                        {
                            case Width::Four:
                                nr = ((buffer[0] & 0x08) ? -1 : 0);
                                nr |= buffer[0];
                                break;
                            case Width::Five:
                                nr = ((buffer[0] & 0x10) ? -1 : 0);
                                nr |= buffer[0];
                                break;
                            case Width::Seven:
                                nr = ((buffer[0] & 0x40) ? -1 : 0);
                                nr |= buffer[0];
                                break;
                            case Width::Eight:
                                nr = ((buffer[0] & 0x80) ? -1 : 0);
                                nr |= buffer[0];
                                break;
                            case Width::Sixteen:
                                MSS(sizeof(Size) >= 2, TooLarge);
                                nr = ((buffer[1] & 0x80) ? -1 : 0);
                                nr |= buffer[1];
                                nr <<= 8; nr |= buffer[0];
                                break;
                            case Width::Thirtytwo:
                                MSS(sizeof(Size) >= 4, TooLarge);
                                nr = ((buffer[3] & 0x80) ? -1 : 0);
                                nr |= buffer[3];
                                nr <<= 8; nr |= buffer[2];
                                nr <<= 8; nr |= buffer[1];
                                nr <<= 8; nr |= buffer[0];
                                break;
                            case Width::Sixtyfour:
                                MSS(sizeof(Size) >= 8, TooLarge);
                                nr = ((buffer[7] & 0x80) ? -1 : 0);
                                nr |= buffer[7];
                                nr <<= 8; nr |= buffer[6];
                                nr <<= 8; nr |= buffer[5];
                                nr <<= 8; nr |= buffer[4];
                                nr <<= 8; nr |= buffer[3];
                                nr <<= 8; nr |= buffer[2];
                                nr <<= 8; nr |= buffer[1];
                                nr <<= 8; nr |= buffer[0];
                                break;
                            default:
                                MSS_L(UnsupportedWidth);
                        }
                        MSS_END();
                    }

                Path path_;
                Element el_;
                Buffer buffer_;
                int nrBytes_;
        };
    }
}

#endif
