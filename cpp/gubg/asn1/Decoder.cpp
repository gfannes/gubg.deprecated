#include "asn1/Decoder.hpp"
#include "asn1/Error.hpp"
using namespace gubg::asn1;
using namespace std;

#define L_ENABLE_DEBUG
#include "debug.hpp"

Decoder::Decoder():
    parent_(0),
    isBlocked_(false){}
Decoder::Decoder(const string &der)
{
    reset(der);
}
Decoder::~Decoder()
{
    //TODO::If the decoder is empty, we assume everything was ok, and we can proceed the parent
    if (parent_)
        parent_->isBlocked_ = false;
}

Decoder Decoder::createSubDecoder_(Range &subRange)
{
    Decoder res(*this);
    res.setRange_(subRange);
    res.parent_ = this;
    isBlocked_ = true;
    return res;
}

void Decoder::reset(const string &der)
{
    der_.reset(new string(der));
    range_ = Range(*der_);
    parent_ = 0;
    isBlocked_ = false;
}

template <>
bool Decoder::decode<int>(int &res)
{
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
        res = (res << 8) + vi.content[i];
    //We successfuly read an integer, proceed range_ to the end of the content
    proceedToEnd_(range_, vi.content.end());
    return true;
}

template <>
bool Decoder::decode<string>(string &res)
{
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
bool Decoder::decode<Decoder>(Decoder &subDecoder)
{
    ValueInfo vi;
    if (!decompose_(vi, range_))
        return false;
    //Check that we have a sequence
    if (0x10 != vi.tag)
        return false;
    subDecoder = createSubDecoder_(vi.content);
    Exception::raise(NotImplemented("WIP"));
    return true;
    //We successfuly read an integer, proceed range_ to the end of the content
    proceedToEnd_(range_, vi.content.end());
    return true;
}

//Private methods
bool Decoder::decompose_(ValueInfo &vi, Range &range)
{
    if (range.empty())
    {
        DEBUG_PRINT("Decoder::decompose_(): range is empty");
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
            DEBUG_PRINT("Decoder::decompose_(): content is empty");
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
                    DEBUG_PRINT("Decoder::decompose_(): contentLength is too small: " << contentLength << " (should be at least " << octet+1 << ")");
                    return false;
                }
                unsigned int length = 0;
                for (int i = 0; i < octet; ++i)
                    length = (length << 8) + content[i+1];
                if (contentLength < octet+1+length)
                {
                    DEBUG_PRINT("Decoder::decompose_(): contentLength is too small: " << contentLength << " (should be at least " << octet+1+length << ")");
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
                DEBUG_PRINT("Decoder::decompose_(): not enough content, I expected " << octet+1 << " bytes, but I have only " << content.size());
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
