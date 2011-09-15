#include "asn1/Encoder.hpp"
#include "asn1/Error.hpp"
using namespace gubg::asn1;
using namespace std;

void Encoder::clear()
{
    oss_.str(Octets());
}

Octets Encoder::encode() const
{
    return oss_.str();
}

template <>
void Encoder::append<int>(const int &i)
{
    addByte_(0x02);
    if (-128 <= i && i <= 127)
        addByte_(0x01).addByte_(i);
    else if (-32768 <= i && i <= 32767)
        addByte_(0x02).addByte_(i >> 8).addByte_(i & 0xff);
}
template <>
void Encoder::append<string>(const string &str)
{
    Encoder::append(str, StringType::IA5);
}

void Encoder::append(const string &str, StringType type)
{
    switch (type)
    {
        case StringType::IA5: addByte_(0x16); break;
        case StringType::Octet: addByte_(0x04); break;
    }
    addLength_(str.size());
    addBytes_(str);
}
void Encoder::append(const char *str, StringType type)
{
    append(string(str), type);
}

void Encoder::append(const Encoder &encoder, StructuredType type)
{
    switch (type)
    {
        case StructuredType::Sequence: addByte_(0x30); break;
        case StructuredType::Set:      addByte_(0x31); break;
    }
    const Octets content = encoder.encode();
    addLength_(content.size()).addBytes_(content);
}

//Private methods
Encoder &Encoder::addLength_(unsigned int length)
{
    if (0 == length)
        addByte_(0);
    else if (1<= length && length <= 127)
        addByte_(length);
    else if (128 <= length && length <= 32767)
        addByte_(0x7f & length >> 8).addByte_(0xff & length);
    else if (32768 <= length && length <= 8388607)
        addByte_(0x7f & length >> 16).addByte_(0xff & length >> 8).addByte_(0xff & length);
    else if (8388608 <= length && length <= 2147483647)
        addByte_(0x7f & length >> 24).addByte_(0xff & length >> 16).addByte_(0xff & length >> 8).addByte_(0xff & length);
    else
        Exception::raise(Error("I can only handle a length of 31 bits"));
    return *this;
}
