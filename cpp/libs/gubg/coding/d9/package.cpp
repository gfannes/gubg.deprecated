#include "gubg/coding/d9/package.hpp"
#include "gubg/coding/d9/rle.hpp"
using namespace std;
using gubg::coding::d9::ubyte;

#include <iostream>
#define L(m) cout<<m<<endl

namespace
{
    using namespace gubg::coding::d9;

    size_t nrDXBytesInBuffer_(const string &plain)
    {
        size_t nr = 0;
        auto end = plain.end();
        for (auto it = plain.begin(); it != end; ++it)
        {
            const unsigned char byte = *it;
            if (Byte::D8 == byte || Byte::D9 == byte)
                ++nr;
        }
        return nr;
    }
    void encodeInBlockFormat_(gubg::coding::d9::rle::Bits &alterations, string &coded, const string &plain)
    {
        alterations.clear();
        coded.resize(plain.size());

        int nrAlterationsInByte;
        string::iterator co = coded.begin();

        //Copy plain into coded (using co), changing D9 into D8, and updating the alterations if necessary
        auto end = plain.end();
        for (auto it = plain.begin(); it != end; ++it)
        {
            const ubyte b = *it;
            switch (b)
            {
                case Byte::D9:
                    alterations.add(true);
                    *(co++) = Byte::D8;
                    break;
                case Byte::D8: 
                    alterations.add(false);
                    *(co++) = Byte::D8;
                    break;
                default:
                    *(co++) = b;
                    break;
            }
        }
    }

    struct EscapeSequence
    {
        size_t nr;
        unsigned char alterations;

        EscapeSequence():
            nr(0), alterations(0){}

        void addD8(){++nr;}
        void addD9(){alterations |= (1 << (nr++));}
        void writeAndReset(ostream &os)
        {
            if (!nr)
                return;
            os << Byte::D8 << (unsigned char)((nr << 5) | alterations);
            nr = 0;
            alterations = 0;
        }
        bool isFull() const {return nr >= 5;}
    };
    void encodeInStreamFormat_(string &coded, const string &plain)
    {
        ostringstream oss;
        auto end = plain.end();
        EscapeSequence es;
        for (auto it = plain.begin(); it != end; ++it)
        {
            const unsigned char byte = *it;
            switch (byte)
            {
                case Byte::D8: es.addD8(); break;
                case Byte::D9: es.addD9(); break;
                default:
                         es.writeAndReset(oss);
                         oss << byte;
                         break;
            }
            if (es.isFull())
                es.writeAndReset(oss);
        }
        es.writeAndReset(oss);
        coded = oss.str();
    }

    ReturnCode decodeFromBlockFormat_(string &plain, const string &coded)
    {
        MSS_BEGIN(ReturnCode);
        Ixs ixs;
        {
            int i = 0;
            for (; i < coded.size(); ++i)
            {
                ubyte b = coded[i];
                MSS((b&0xc0) != 0xc0, RLEIllegalMSBits);
                if (b & 0x80)
                    break;
            }
            MSS(rle::Bits::decode(ixs, coded.substr(0, i+1)));
            plain = coded.substr(i+1);
        }
        auto ix = ixs.begin();
        auto ixend = ixs.end();
        int nrDX = 0;
        auto end = plain.end();
        for (auto it = plain.begin(); it != end; ++it)
        {
            ubyte b = *it;
            if (b == Byte::D8)
            {
                if (ix != ixend && nrDX == *ix)
                {
                    *it = Byte::D9;
                    ++ix;
                }
                ++nrDX;
            }
        }
        MSS_END();
    }

    ReturnCode decodeFromStreamFormat_(string &plain, const string &coded)
    {
        MSS_BEGIN(ReturnCode);
        ostringstream oss;
        auto end = coded.end();
        for (auto it = coded.begin(); it != end; ++it)
        {
            ubyte b = *it;
            if (b == Byte::D8)
            {
                ++it;
                MSS(it != end, IllegalEscapeSequence);
                b = *it;
                int nr = (b>>5);
                for (int i = 0; i < nr; ++i, b >>= 1)
                    oss << (b&0x01 ? Byte::D9 : Byte::D8);
            }
            else
                oss << b;
        }
        plain = oss.str();
        MSS_END();
    }

    struct ChecksumStream
    {
        ChecksumStream(ostream &o):
            os(o), checksum(0){}
        ChecksumStream &operator<<(ubyte b)
        {
            checksum ^= b;
            os << b;
            return *this;
        }
        ChecksumStream &operator<<(const string &data)
        {
            auto end = data.end();
            for (auto it = data.begin(); it != end; ++it)
                operator<<((ubyte)*it);
            return *this;
        }
        ubyte checksum;
        ostream &os;
    };

    class Attributes
    {
        public:
            void add(unsigned long attrib){oss_ << gubg::coding::d9::rle::encodeNumber(attrib);}
            void add(unsigned long first, unsigned long second){oss_ << gubg::coding::d9::rle::encodePair(first, second);}
            string encode() const {return oss_.str();}
        private:
            ostringstream oss_;
    };

    ReturnCode readUntil_(string &str, const ubyte wanted, IInput &input)
    {
        MSS_BEGIN(ReturnCode);
        str.clear();
        ubyte b;
        while (true)
        {
            MSS(input.read(b));
            if (b == wanted)
                break;
            str.push_back(b);
        }
        MSS_END();
    }
}

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            //Packge
            const static long AddressNotSet = -1;
            const static long IdNotSet = -1;
            Package::Package():
                checksum_(true),
                version_(0),
                format_(Format::Unknown),
                contentType_(ContentType::NoContent),
                src_(AddressNotSet),
                dst_(AddressNotSet),
                id_(IdNotSet){}

            //Setters
            Package &Package::checksum(bool b){checksum_ = b; return *this;}
            Package &Package::format(Format format){format_ = format; return *this;}
            Package &Package::content(string c, ContentType contentType)
            {
                contentType_ = contentType;
                content_ = std::move(c);
                return *this;
            }
            Package &Package::content(string &&c, ContentType contentType)
            {
                contentType_ = contentType;
                content_ = std::move(c);
                return *this;
            }
            Package &Package::source(Address address){src_ = address; return *this;}
            Package &Package::destination(Address address){dst_ = address; return *this;}
            Package &Package::id(Id i){id_ = i; return *this;}

            //Getters
            ReturnCode Package::getContent(string &plain) const
            {
                MSS_BEGIN(ReturnCode);
                MSS(contentType_ != ContentType::NoContent, NoContentPresent);
                plain = content_;
                MSS_END();
            }

            ReturnCode Package::encode(string &coded) const
            {
                MSS_BEGIN(ReturnCode);
                ostringstream oss;
                rle::Bits meta;
                Attributes attributes;

                //Checksum
                meta.add(checksum_);

                //Version
                {
                    MSS(version_ == 0, UnsupportedVersion);
                    meta.add(false);
                }

                string d9FreeBuffer;
                const bool hasContent = contentType_ != ContentType::NoContent;
                if (hasContent)
                {
                    Format realFormat = format_;
                    switch (format_)
                    {
                        case Format::AsIs:
                        case Format::Block:
                            {
                                rle::Bits alterations;
                                string coded;
                                encodeInBlockFormat_(alterations, coded, content_);
                                switch (format_)
                                {
                                    case Format::AsIs:
                                        MSS(alterations.empty(), AlterationsNotAllowedForAsIs);
                                        d9FreeBuffer.append(coded);
                                        break;
                                    case Format::Block:
                                        if (alterations.empty())
                                        {
                                            //When no 0dx8/9 characters are used, we can save on the alterations by switching to Format::AsIs
                                            realFormat = (alterations.empty() ? Format::AsIs : Format::Block);
                                            d9FreeBuffer.append(coded);
                                        }
                                        else
                                        {
                                            d9FreeBuffer.append(alterations.encode());
                                            d9FreeBuffer.append(coded);
                                        }
                                        break;
                                }
                            }
                            break;
                        case Format::Stream:
                            encodeInStreamFormat_(d9FreeBuffer, content_);
                            break;
                        default: MSS_L(UnknownFormat); break;
                    }
                    meta.add(true);//Content
                    attributes.add((unsigned long)realFormat, (unsigned long)contentType_);
                }
                else
                {
                    meta.add(false);//Content
                }

                //Source
                if (src_ >= 0)
                {
                    meta.add(true);
                    attributes.add(src_);
                }
                else
                    meta.add(false);

                //Destination
                if (dst_ >= 0)
                {
                    meta.add(true);
                    attributes.add(dst_);
                }
                else
                    meta.add(false);

                //PackageId
                if (id_ >= 0)
                {
                    meta.add(true);
                    attributes.add(id_);
                }
                else
                    meta.add(false);

                //Add all collected info and compute a checksum
                {
                    ChecksumStream coss(oss);
                    coss << Byte::D9;
                    coss << meta.encode();
                    coss << attributes.encode();
                    if (hasContent)
                    {
                        coss << d9FreeBuffer;
                        coss << Byte::D9;
                        coss << Byte::End;
                    }

                    if (checksum_)
                        //Added the 7 lsbits of the checksum
                        oss << ubyte(coss.checksum & 0x7f);
                }

                coded = oss.str();
                MSS_END();
            }
            ReturnCode Package::decode(const string &coded)
            {
                Input<FromString> input(coded);
                return decode(input);
            }
            ReturnCode Package::decode(IInput &input)
            {
                MSS_BEGIN(ReturnCode);
                ubyte b;
                MSS(input.read(b), PackageTooSmall);
                MSS(b == Byte::D9, MissingStart);
                Ixs ixs;
                MSS(rle::Bits::decode(ixs, input), CannotDecodeMeta);
                clear_();
                for (auto ix = ixs.begin(); ix != ixs.end(); ++ix)
                {
                    switch ((Meta)*ix)
                    {
                        case Meta::Checksum: checksum_ = true; break;
                        case Meta::Version: MSS_L(UnsupportedVersion); break;
                        case Meta::Content:
                                            {
                                                unsigned long format, type;
                                                MSS(rle::decodePair(format, type, input), CannotDecodeContentMeta);
                                                format_ = (Format)format;
                                                contentType_ = (ContentType)type;
                                            }
                                            break;
                        case Meta::Source: MSS(rle::decodeNumber(src_, input), CannotDecodeSource); break;
                        case Meta::Destination: MSS(rle::decodeNumber(src_, input), CannotDecodeDestination); break;
                        case Meta::PackageId: MSS(rle::decodeNumber(src_, input), CannotDecodeId); break;
                        default: MSS_L(UnknownMetaField); break;
                    }
                }
                if (contentType_ != ContentType::NoContent)
                {
                    string coded;
                    MSS(readUntil_(coded, Byte::D9, input), CannotReadCodedContent);
                    {
                        ubyte b;
                        MSS(input.read(b), ExpectedEndByte);
                        MSS(b == Byte::End, ExpectedEndByte);
                    }
                    switch (format_)
                    {
                        case Format::AsIs: content_ = std::move(coded); break;
                        case Format::Block: MSS(decodeFromBlockFormat_(content_, coded)); break;
                        case Format::Stream: MSS(decodeFromStreamFormat_(content_, coded)); break;
                        default: MSS_L(IllegalFormat); break;
                    }
                }
                if (checksum_)
                {
                    //Checksum us not checked yet
                    ubyte checksum = 0;
                    MSS(input.read(checksum), ExpectedChecksum);
                }
                MSS_END();
            }
            void Package::clear_()
            {
                checksum_ = false;
                version_ = 0;
                format_ = Format::Unknown;
                contentType_ = ContentType::NoContent;
                src_ = AddressNotSet;
                dst_ = AddressNotSet;
                id_ = IdNotSet;
            }
        }
    }
}