#include "asn1/Decoder.hpp"
#include "asn1/Error.hpp"
#include "gubg/l.hpp"
using namespace gubg::asn1;
using namespace std;

Decoder::Decoder():
    parent_(nullptr),
    isBlocked_(false),
    decodingStatus_(DecodingStatus::Unknown)
{
    L("Decoder default ctor: " << this);
}
Decoder::Decoder(const string &der)
{
    L("Decoder ctor from string: " << this);
    reset(der);
}
Decoder::Decoder(const Octets &der)
{
    L("Decoder ctor from Octets: " << this);
    reset(der);
}
Decoder::~Decoder()
{
    if (parent_)
    {
        switch (decodingStatus_)
        {
            case DecodingStatus::Success:
                proceedToEnd_(parent_->range_, range_.end());
                //Fall-through is intentional
            case DecodingStatus::Failure:
                L("Unblocking decoder " << parent_ << " in dtor " << this);
                parent_->isBlocked_ = false;
                break;
            default:
                //We won't unblock the parent, an unknown DecodingStatus is present
                cout << "You forgot to set the DecodingStatus for decoder " << this << endl;
                break;
        }
    }
}

void Decoder::createSubDecoder_(Decoder &subdecoder, Range &subRange)
{
    subdecoder.der_ = der_;
    subdecoder.range_ = subRange;
    subdecoder.parent_ = this;
    subdecoder.isBlocked_ = false;
    subdecoder.decodingStatus_ = DecodingStatus::Unknown;
    L("Blocking decoder " << this);
    isBlocked_ = true;
}

void Decoder::reset(const Octets &der)
{
    der_.reset(new Octets(der));
    range_ = Range(*der_);
    parent_ = nullptr;
    L("Unblocking decoder in reset " << this);
    isBlocked_ = false;
    decodingStatus_ = DecodingStatus::Unknown;
}
void Decoder::reset(const string &der)
{
    der_.reset(new Octets((const unsigned char *)der.data(), der.size()));
    range_ = Range(*der_);
    parent_ = nullptr;
    L("Unblocking decoder in reset " << this);
    isBlocked_ = false;
    decodingStatus_ = DecodingStatus::Unknown;
}

template <>
bool Decoder::extract<int>(int &res)
{
    L("Extracting an int");
    checkNotBlocked_();
    ValueInfo vi;
    if (!decompose_(vi, range_))
        return false;
    //Check that we have an integer
    if (0x02 != vi.tag)
        return false;
    auto contentSize = vi.content.size();
    if (contentSize > 4)
        //We cannot handle integer more that 4 bytes
        return false;
    //Set the res value
    res = 0;
    for (int i = 0; i < contentSize; ++i)
    {
        unsigned char byte = (unsigned char)vi.content[i];
        if (0 == i && byte & 0x80)
            res = -1;
        res = (res << 8) | byte;
    }
    //We successfuly read an integer, proceed range_ to the end of the content
    proceedToEnd_(range_, vi.content.end());
    return true;
}

template <>
bool Decoder::extract<string>(string &res)
{
    checkNotBlocked_();
    ValueInfo vi;
    if (!decompose_(vi, range_))
        return false;
    switch (vi.tag)
    {
        case 0x04://Octet string
        case 0x16://IA5String
            res = string(vi.content.begin(), vi.content.end());
            break;
        default:
            return false;
            break;
    }
    //We successfuly read an integer, proceed range_ to the end of the content
    proceedToEnd_(range_, vi.content.end());
    return true;
}
template <>
bool Decoder::extract<Octets>(Octets &res)
{
    checkNotBlocked_();
    ValueInfo vi;
    if (!decompose_(vi, range_))
        return false;
    switch (vi.tag)
    {
        case 0x04://Octet string
            res = Octets(vi.content.begin(), vi.content.end());
            break;
        default:
            return false;
            break;
    }
    //We successfuly read an integer, proceed range_ to the end of the content
    proceedToEnd_(range_, vi.content.end());
    return true;
}

template <>
bool Decoder::extract<Decoder>(Decoder &subdecoder)
{
    checkNotBlocked_();
    ValueInfo vi;
    if (!decompose_(vi, range_))
        return false;
    //Check that we have a sequence
    if (0x10 != vi.tag)
        return false;
    createSubDecoder_(subdecoder, vi.content);
    //this->range_ will be adjusted when the subdecoder destructs, only if you indicated that decoding was OK via indicateDecodingOK()
    return true;
}

void Decoder::indicateDecodingOK(bool ok)
{
    decodingStatus_ = (ok ? DecodingStatus::Success : DecodingStatus::Failure);
}

//Private methods
bool Decoder::decompose_(ValueInfo &vi, Range &range)
{
    if (range.empty())
    {
        L("Decoder::decompose_(): range is empty");
        return false;
    }
    //We will further alter the content range to be correct at the end
    Range &content = vi.content;
    content = range;
    {
        unsigned char identifier = content[0];
        content.advance_begin(1);
        switch (identifier & 0xc0)
        {
            case 0x00: vi.klass = Class::Universal; break;
            case 0x40: vi.klass = Class::Application; break;
            case 0x80: vi.klass = Class::ContextSpecific; break;
            case 0xc0: vi.klass = Class::Private; break;
        }
        vi.isConstructed = identifier & 0x20;
        vi.tag = identifier & 0x1f;
    }
    //Move content to the actual content (reading length etc.)
    {
        if (content.empty())
        {
            L("Decoder::decompose_(): content is empty");
            return false;
        }
        unsigned char octet = content[0];
        if (octet & 0x80)
        {
            //Length is specified in long form
            octet &= 0x7f;
            if (octet)
            {
                //Definite length
                if (octet > 4)
                    Exception::raise(Error("Cannot handle length of more that 4 octets"));
                unsigned int contentLength = content.size();
                if (contentLength < octet+1)
                {
                    L("Decoder::decompose_(): contentLength is too small: " << contentLength << " (should be at least " << octet+1 << ")");
                    return false;
                }
                unsigned int length = 0;
                for (int i = 0; i < octet; ++i)
                    length = (length << 8) + content[i+1];
                if (contentLength < octet+1+length)
                {
                    L("Decoder::decompose_(): contentLength is too small: " << contentLength << " (should be at least " << octet+1+length << ")");
                    return false;
                }
                auto start = content.begin() + octet+1;
                content = Range(start, start + length);
            }
            else
            {
                //Indefinite length
                Exception::raise(NotImplemented("Cannot handle indefinite lengths yet"));
            }
        }
        else
        {
            //Length is specified in short form
            octet &= 0x7f;
            if (content.size() < octet+1)
            {
                L("Decoder::decompose_(): not enough content, I expected " << octet+1 << " bytes, but I have only " << content.size());
                return false;
            }
            auto start = content.begin()+1;
            content = Range(start, start + octet);
        }
    }
    return true;
}
void Decoder::proceedToEnd_(Range &range, const Iterator &end)
{
    range = Range(end, range.end());
}
void Decoder::checkNotBlocked_() const throw (Decoder::Blocked)
{
    L("checkNotBlocked_: " << isBlocked_);
    if (isBlocked_)
    {
        L("Decoder is blocked, I will throw Decoder::Blocked");
        Exception::raise(Blocked());
    }
}
