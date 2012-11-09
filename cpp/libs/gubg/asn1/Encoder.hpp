#ifndef HEADER_gubg_asn1_Encoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_asn1_Encoder_hpp_ALREADY_INCLUDED

#include "asn1/Types.hpp"
#include <string>
#include <sstream>
#include <vector>

namespace gubg
{
    namespace asn1
    {
        enum class StringType {IA5, Octet};
        enum class StructuredType {Sequence, Set};
        class Encoder
        {
            public:
                void clear();

                //This is the most general template
                template <typename T>
                void append(const T &t)
                {
                    t.serialize(*this);
                }
                template <typename T>
                void append(const std::vector<T> &vec)
                {
                    Encoder encoder;
                    for (const auto &v: vec)
                        encoder.append(v);
                    auto content = encoder.encode();
                    addByte_(0x30).addLength_(content.size()).addBytes_(content);
                }

                //Template only used for inline strings
                //I'm not really sure why, but this seems to work
                template <typename T>
                void append(T const * t)
                {
                    append(t, StringType::IA5);
                }

                void append(const std::string &str, StringType);
                void append(const char *str, StringType);

                void append(const Encoder &encoder, StructuredType);

                Octets encode() const;

            private:
                Encoder &addByte_(unsigned char ch){oss_ << ch; return *this;}
                Encoder &addBytes_(const std::string &str){oss_.write((const unsigned char *)str.data(), str.size()); return *this;}
                Encoder &addBytes_(const Octets &str){oss_ << str; return *this;}
                Encoder &addLength_(unsigned int length);
                std::basic_ostringstream<unsigned char> oss_;
        };
        template <>
            void Encoder::append<int>(const int &i);
        template <>
            void Encoder::append<std::string>(const std::string &str);
    }
}

#endif
