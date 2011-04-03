#ifndef gubg_asn1_Encoder_hpp
#define gubg_asn1_Encoder_hpp

#include <string>
#include <sstream>

namespace gubg
{
    namespace asn1
    {
        enum class StringType {Octet, IA5};
        class Encoder
        {
            public:
                void clear();

                template <typename T>
                void encode(const T &t);

                void encode(const std::string &str, StringType type);

                std::string toString() const;

            private:
                Encoder &addByte_(char ch){oss_ << ch; return *this;}
                Encoder &addBytes_(const std::string &str){oss_ << str; return *this;}
                Encoder &addLength_(unsigned int length);
                std::ostringstream oss_;
        };
        template <>
            void Encoder::encode<int>(const int &i);
    }
}

#endif
