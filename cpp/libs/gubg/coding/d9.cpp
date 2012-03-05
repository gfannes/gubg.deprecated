#include "gubg/coding/d9.hpp"
using namespace std;
using gubg::coding::ubyte;

#include <iostream>
#define L(m) cout<<m<<endl

namespace
{
    const ubyte D8 = 0xd8;
    const ubyte D9 = 0xd9;
    const ubyte EndByte = 0xff;

    size_t nrDXBytesInBuffer_(const string &plain)
    {
        size_t nr = 0;
        auto end = plain.end();
        for (auto it = plain.begin(); it != end; ++it)
        {
            const unsigned char byte = *it;
            if (D8 == byte || D9 == byte)
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
                case D9:
                    alterations.add(true);
                    *(co++) = D8;
                    break;
                case D8: 
                    alterations.add(false);
                    *(co++) = D8;
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
            os << D8 << (unsigned char)((nr << 5) | alterations);
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
                case D8: es.addD8(); break;
                case D9: es.addD9(); break;
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

    template <typename Source>
        string convertBytesToString_(Source &&bytes)
        {
            string res(bytes.size(), 0);
            auto end = bytes.end();
            size_t i = 0;
            for (auto it = bytes.begin(); it != end; ++it, ++i)
                res[i] = *it;
            return std::move(res);
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

    bool read_(ubyte &b, istream &is)
    {
        return is >> b;
    }
}

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            namespace rle
            {
                string encodeNumber(unsigned long v)
                {
                    if (v <= 0x3f)
                        return convertBytesToString_({0x80 | v});
                    if (v <= 0x1fff)
                        return convertBytesToString_({(v&0x1fff)>>6, (v&0x3f)|0x80});
                    if (v <= 0xfffff)
                        return convertBytesToString_({(v&0xfffff)>>13, (v&0x1fff)>>6, (v&0x3f)|0x80});
                    if (v <= 0x7ffffff)
                        return convertBytesToString_({(v&0x7ffffff)>>20, (v&0xfffff)>>13, (v&0x1fff)>>6, (v&0x3f)|0x80});
                    return convertBytesToString_({v>>27, (v&0x7ffffff)>>20, (v&0xfffff)>>13, (v&0x1fff)>>6, (v&0x3f)|0x80});
                }
                ReturnCode decodeNumber(unsigned long &v, const string &coded)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS_T(!coded.empty(), RLETooSmall);
                    MSS_T(coded.size() <= 5, RLETooLarge);
                    v = 0;
                    auto end = coded.end();
                    for (auto it = coded.begin(); it != end; ++it)
                    {
                        ubyte b = *it;
                        MSS_T((b&0xc0)!=0xc0, RLEIllegalMSBits);
                        if (b&0x80)
                        {
                            //This is the closing byte of the RLE
                            MSS_T(it+1 == end, RLEClosingByteExpected);
                            v <<= 6;
                            v |= (0x3f&b);
                        }
                        else
                        {
                            v <<= 7;
                            v |= (0x7f&b);
                        }
                    }
                    MSS_END();
                }
                string encodePair(unsigned long first, unsigned long second)
                {
                    if (first <= 0x03 && second <= 0x0f)
                        return convertBytesToString_({0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x1f && second <= 0xff)
                        return convertBytesToString_({
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0xff && second <= 0xfff)
                        return convertBytesToString_({
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x7ff && second <= 0xffff)
                        return convertBytesToString_({
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x3fff && second <= 0xfffff)
                        return convertBytesToString_({
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x1ffff && second <= 0xffffff)
                        return convertBytesToString_({
                                (0x70 & (first>>15)<<4)  | (0x0f & second>>20),
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0xfffff && second <= 0xfffffff)
                        return convertBytesToString_({
                                (0x70 & (first>>18)<<4)  | (0x0f & second>>24),
                                (0x70 & (first>>15)<<4)  | (0x0f & second>>20),
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x7fffff && second <= 0xffffffff)
                        return convertBytesToString_({
                                (0x70 & (first>>21)<<4)  | (0x0f & second>>28),
                                (0x70 & (first>>18)<<4)  | (0x0f & second>>24),
                                (0x70 & (first>>15)<<4)  | (0x0f & second>>20),
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x3ffffff && second <= 0xfffffffff)
                        return convertBytesToString_({
                                (0x70 & (first>>24)<<4)  | (0x0f & second>>32),
                                (0x70 & (first>>21)<<4)  | (0x0f & second>>28),
                                (0x70 & (first>>18)<<4)  | (0x0f & second>>24),
                                (0x70 & (first>>15)<<4)  | (0x0f & second>>20),
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    return "you are probably abusing pair";
                }
                ReturnCode decodePair(unsigned long &first, unsigned long &second, const string &coded)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS_T(!coded.empty(), RLETooSmall);
                    MSS_T(coded.size() <= 9, RLETooLarge);
                    first = second = 0;
                    auto end = coded.end();
                    for (auto it = coded.begin(); it != end; ++it)
                    {
                        ubyte b = *it;
                        MSS_T((b&0xc0)!=0xc0, RLEIllegalMSBits);
                        if (b&0x80)
                        {
                            //This is the closing byte of the RLE
                            MSS_T(it+1 == end, RLEClosingByteExpected);
                            first <<= 2;
                            first |= (0x03&b>>4);
                            second <<= 4;
                            second |= (0x0f&b);
                        }
                        else
                        {
                            first <<= 3;
                            first |= (0x07&b>>4);
                            second <<= 4;
                            second |= (0x0f&b);
                        }
                    }
                    MSS_END();
                }

                //Bits
                Bits::Bits():
                    current(0), nr(0){}
                void Bits::add(bool b)
                {
                    if (b)
                        current |= (1 << nr);
                    const int nrBits = (buffer.empty() ? 6 : 7);
                    if (++nr >= nrBits)
                        appendCurrentToBuffer_();
                }
                string Bits::encode() const
                {
                    string res;
                    if (buffer.empty())
                    {
                        res.push_back(current | 0x80);
                    }
                    else
                    {
                        if (nr > 0)
                            res.push_back(current);
                        for (auto it = buffer.rbegin(); it != buffer.rend(); ++it)
                            res.push_back(*it);
                    }
                    return std::move(res);
                }
                ReturnCode Bits::decode(Ixs &ixs, const string &coded)
                {
                    MSS_BEGIN(ReturnCode);
                    ixs.clear();
                    unsigned long offset = 0;
                    auto end = coded.rend();
                    for (auto it = coded.rbegin(); it != end; ++it)
                    {
                        ubyte b = *it;
                        const int nrBits = (b & 0x80 ? 6 : 7);
                        for (int i = 0; i < nrBits; ++i)
                            if (b & 1<<i)
                                ixs.push_back(offset+i);
                        offset += nrBits;
                    }
                    MSS_END();
                }
                ReturnCode Bits::decode(Ixs &ixs, istream &is)
                {
                    MSS_BEGIN(ReturnCode);
                    //Read a RLE encoded bits into a string
                    string coded;
                    {
                        while (true)
                        {
                            ubyte b;
                            MSS_T(read_(b, is), UnexpectedBitsEnd);
                            MSS_T((b&0xc0) != 0xc0, RLEIllegalMSBits);
                            coded.push_back(b);
                            if (0x80 & b)
                                //This is the end-byte of the RLE
                                break;
                        }
                    }
                    //Convert the coded string into ixs
                    MSS(decode(ixs, coded));
                    MSS_END();
                }
                void Bits::clear()
                {
                    nr = current = 0;
                    buffer.clear();
                }
                bool Bits::empty() const
                {
                    return buffer.empty() && nr == 0;
                }
                void Bits::appendCurrentToBuffer_()
                {
                    if (buffer.empty())
                        current |= 0x80;
                    buffer.push_back(current);
                    nr = 0;
                    current = 0;
                }
            }

            string to_s( Format format)
            {
                switch (format)
                {
                    case Format::Block:  return "block format"; break;
                    case Format::Stream: return "stream format"; break;
                }
                return "unknown format";
            }

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
                MSS_T(contentType_ != ContentType::NoContent, NoContentPresent);
                plain = content_;
                MSS_END();
            }

            ReturnCode Package::encode(string &coded) const
            {
                MSS_BEGIN(ReturnCode);
                ostringstream oss;
                const bool hasContent = contentType_ != ContentType::NoContent;
                //Add everything that is covered by the checksum
                {
                    ChecksumStream coss(oss);
                    coss << D9;
                    rle::Bits meta;
                    Attributes attributes;

                    //Checksum
                    meta.add(checksum_);

                    //Version
                    {
                        MSS_T(version_ == 0, UnsupportedVersion);
                        meta.add(false);
                    }

                    string d9FreeBuffer;
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
                                            MSS_T(alterations.empty(), AlterationsNotAllowedForAsIs);
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

                    coss << meta.encode();
                    coss << attributes.encode();
                    if (hasContent)
                        coss << d9FreeBuffer;
                    if (checksum_)
                        //Added the 7 lsbits of the checksum
                        oss << ubyte(coss.checksum & 0x7f);
                }
                if (hasContent)
                {
                    oss << D9;
                    oss << EndByte;
                }
                coded = oss.str();
                MSS_END();
            }
            ReturnCode Package::decode(const string &coded)
            {
                //For now, we use the streamed version for reading
                istringstream iss(coded);
                return decode(iss);
            }
            ReturnCode Package::decode(istream &is)
            {
                MSS_BEGIN(ReturnCode);
                ubyte b;
                MSS_T(read_(b, is), PackageTooSmall);
                MSS_T(b == D9, MissingStart);
                Ixs ixs;
                MSS_T(rle::Bits::decode(ixs, is), CannotDecodeMeta);
                MSS_END();
            }
        }
    }
}

