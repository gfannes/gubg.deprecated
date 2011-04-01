#ifndef gubg_ASN1_hpp
#define gubg_ASN1_hpp

#include "Exception.hpp"
#include "boost/range/iterator_range.hpp"
#include <string>

namespace gubg
{
    namespace asn1
    {
        struct ASN1Error: Exception
        {
            ASN1Error(const std::string &msg):
                Exception(msg){}
        };
        //DER-specific decoder
        class Decoder
        {
            public:
                Decoder(const std::string &der);
                void reset(const std::string &der);

                //We don't know how to construct a general object, the object should construct itself
                template <typename T>
                bool decode(T &t)
                {
                    auto restoreRange = range_;
                    if (!t.construct(*this))
                    {
                        //If construction failed, we restore the original range
                        range_ = restoreRange;
                        return false;
                    }
                    return true;
                }

            private:
                typedef std::string::const_iterator Iterator;
                typedef boost::iterator_range<Iterator> Range;
                //TODO::These things can be moved into an anonymous namespace in the source file
                enum class Class {Universal, Application, ContextSpecific, Private};
                struct ValueInfo
                {
                    Class klass;
                    bool isConstructed;
                    unsigned int tag;
                    Range content;
                };
                static bool decompose_(ValueInfo &, Range &range);
                static void proceedToEnd_(Range &range, const Iterator &end);

                std::string der_;
                Range range_;
        };
        //Reads an asn.1 Integer
        template <>
            bool Decoder::decode<int>(int &);
        //Reads both an asn.1 OctetString or an IA5String
        template <>
            bool Decoder::decode<std::string>(std::string &);
    }
}

#endif
