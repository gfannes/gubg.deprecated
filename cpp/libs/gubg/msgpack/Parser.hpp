#ifndef HEADER_gubg_msgpack_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Parser_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Codes.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include "gubg/FixedVector.hpp"

#define GUBG_MODULE "msgpack_Parser"
#include "gubg/log/begin.hpp"
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

        template <typename Receiver, typename Path_>
            class Parser_crtp
            {
                public:
                    typedef Path_ Path;

                    //Emits ParsingFinished when a complete top-level object is parsed
                    ReturnCode process(uint8_t b)
                    {
                        MSS_BEGIN(ReturnCode);

                        bool isNewObject = false;

                        if (mss::isOK(el_.type.valid()))
                        {
                            L("Type type is known");
                            //Add this byte to the length buffer, if still needed
                            if (it_ != buffer_.end())
                                *it_++ = b;
                        }
                        else
                        {
                            L("This should start a new object");
                            //We are ready to read a new object
                            MSS(el_.type.read(b));
                            isNewObject = true;
                            switch (el_.type.width)
                            {
                                case Width::Zero:
                                    buffer_.resize(0);
                                    it_ = buffer_.begin();
                                    break;
                                case Width::Four:
                                case Width::Five:
                                case Width::Seven:
                                    buffer_.resize(1);
                                    it_ = buffer_.begin();
                                    *it_++ = el_.type.nr;
                                    break;
                                case Width::Eight:
                                    buffer_.resize(1);
                                    it_ = buffer_.begin();
                                    break;
                                case Width::Sixteen:
                                    buffer_.resize(2);
                                    it_ = buffer_.begin();
                                    break;
                                case Width::Thirtytwo:
                                    buffer_.resize(4);
                                    it_ = buffer_.begin();
                                    break;
                                case Width::Sixtyfour:
                                    buffer_.resize(8);
                                    it_ = buffer_.begin();
                                    break;
                            }
                        }
                        if (it_ == buffer_.end())
                        {
                            L("The type and data len are known, time to read the data");
                            //We read the length info into buffer_, we can now read the data

                            switch (el_.type.group)
                            {
                                case Group::Array:
                                    MSS(convertUInt_(el_.length, buffer_));
                                    el_.ix = 0;
                                    receiver_().parser_open(el_, path_);
                                    path_.push_back(el_);
                                    el_.clear();
                                    break;
                                case Group::Raw:
                                    if (isNewObject)
                                    {
                                        MSS(convertUInt_(el_.length, buffer_));
                                        el_.ix = 0;
                                        receiver_().parser_open(el_, path_);
                                        path_.push_back(el_);
                                    }
                                    else
                                    {
                                        receiver_().parser_add((char)b, path_);
                                        //We do not clear el_ to make sure the next byte won't cause the start of a new object
                                        auto &el = path_.back();
                                        ++el.ix;
                                        L("Proceeding char ix to " << el.ix << " len: " << el.length);
                                        if (el.ix == el.length)
                                        {
                                            receiver_().parser_close(el, path_);
                                            path_.pop_back();
                                            MSS_Q(proceed_());
                                        }
                                    }
                                    break;
                                case Group::Map:
                                    MSS(convertUInt_(el_.length, buffer_));
                                    el_.length += el_.length;
                                    el_.ix = 0;
                                    receiver_().parser_open(el_, path_);
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
                                                MSS(convertUInt_(l, buffer_));
                                                receiver_().parser_add((long)l, path_);
                                                MSS_Q(proceed_());
                                            }
                                            break;
                                        case Primitive::NegFix:
                                            {
                                                long l = el_.type.nr;
                                                l -= 32;
                                                receiver_().parser_add(l, path_);
                                                MSS_Q(proceed_());
                                            }
                                            break;
                                        case Primitive::Int8:
                                        case Primitive::Int16:
                                        case Primitive::Int32:
                                        case Primitive::Int64:
                                            {
                                                long l;
                                                MSS(convertInt_(l, el_.type.width, buffer_));
                                                receiver_().parser_add(l, path_);
                                                MSS_Q(proceed_());
                                            }
                                            break;
                                    }
                                    break;
                                case Group::Nil:
                                    receiver_().parser_add(Nil_tag(), path_);
                                    MSS_Q(proceed_());
                                    break;
								case Group::Boolean:
									receiver_().parser_add(Bool(el_.type.primitive == Primitive::True), path_);
									MSS_Q(proceed_());
									break;
							}
                        }
                        MSS_END();
                    }
                    template <typename String>
                        ReturnCode process(const String &str)
                        {
                            MSS_BEGIN(ReturnCode);
                            for (auto it = str.begin(); it != str.end(); ++it)
                            {
                                switch (auto rc = process((uint8_t)*it))
                                {
                                    case ReturnCode::OK:
                                    case ReturnCode::ParsingFinished:
                                        break;
                                    default:
                                        MSS(rc);
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
                    typedef gubg::FixedVector<uint8_t, 8> Buffer;

                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                    ReturnCode proceed_()
                    {
                        MSS_BEGIN(ReturnCode);
                        el_.clear();
                        MSS_Q(!path_.empty(), ParsingFinished);
                        auto &el = path_.back();
                        ++el.ix;
                        L("Proceeded to " << el.ix << " len: " << el.length);
                        if (el.ix == el.length)
                        {
                            S();L("This was the last of the list, pop and close");
                            auto e = path_.back();
                            path_.pop_back();
                            receiver_().parser_close(e, path_);
							MSS_Q(proceed_());
                        }
                        MSS_END();
                    }
                    template <typename Size>
                        static ReturnCode convertUInt_(Size &nr, const Buffer &buffer)
                        {
                            MSS_BEGIN(ReturnCode);
                            switch (buffer.size())
                            {
                                case 1:
                                    nr = buffer[0];
                                    break;
                                case 2:
                                    MSS(sizeof(Size) >= 2, TooLarge);
                                    nr = buffer[0];
                                    nr <<= 8; nr |= buffer[1];
                                    break;
                                case 4:
                                    MSS(sizeof(Size) >= 4, TooLarge);
                                    nr = buffer[0];
                                    nr <<= 8; nr |= buffer[1];
                                    nr <<= 8; nr |= buffer[2];
                                    nr <<= 8; nr |= buffer[3];
                                    break;
                                case 8:
                                    MSS(sizeof(Size) >= 8, TooLarge);
                                    nr = buffer[0];
                                    nr <<= 8; nr |= buffer[1];
                                    nr <<= 8; nr |= buffer[2];
                                    nr <<= 8; nr |= buffer[3];
                                    nr <<= 8; nr |= buffer[4];
                                    nr <<= 8; nr |= buffer[5];
                                    nr <<= 8; nr |= buffer[6];
                                    nr <<= 8; nr |= buffer[7];
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
                            switch (buffer.size())
                            {
                                case 1:
                                    switch (w)
                                    {
                                        case Width::Four: nr = ((buffer[0] & 0x08) ? -16 : 0); break;
                                        case Width::Seven: nr = ((buffer[0] & 0x40) ? -127 : 0); break;
                                        case Width::Eight: nr = ((buffer[0] & 0x80) ? -256 : 0); break;
                                        default: MSS_L(UnsupportedWidth); break;
                                    }
                                    nr |= (buffer[0] << 0);
                                    break;
                                case 2:
                                    MSS(sizeof(Size) >= 2, TooLarge);
                                    nr = ((buffer[0] & 0x80) ? -65536 : 0);
                                    nr |= (buffer[0] << 8);
                                    nr |= (buffer[1] << 0);
                                    break;
                                case 4:
                                    MSS(sizeof(Size) >= 4, TooLarge);
                                    nr = ((buffer[0] & 0x80) ? -4294967296 : 0);
                                    nr |= (buffer[0] << 24);
                                    nr |= (buffer[1] << 16);
                                    nr |= (buffer[2] << 8);
                                    nr |= (buffer[3] << 0);
                                    break;
                                case 8:
                                    //If this assert fails, you have to initialize nr properly to a negative number, as above
                                    assert(sizeof(Size) == 8);
                                    MSS(sizeof(Size) >= 8, TooLarge);
                                    nr = 0;
                                    nr |= (buffer[0] << 56);
                                    nr |= (buffer[1] << 48);
                                    nr |= (buffer[2] << 40);
                                    nr |= (buffer[3] << 32);
                                    nr |= (buffer[4] << 24);
                                    nr |= (buffer[5] << 16);
                                    nr |= (buffer[6] << 8);
                                    nr |= (buffer[7] << 0);
                                    break;
                            }
                            MSS_END();
                        }

                    Path path_;
                    Element el_;
                    Buffer buffer_;
                    Buffer::iterator it_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
