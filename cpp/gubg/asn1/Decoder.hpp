#ifndef gubg_asn1_Decoder_hpp
#define gubg_asn1_Decoder_hpp

#include "Exception.hpp"
#include "boost/range/iterator_range.hpp"
#include <string>
#include <memory>

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
                Decoder(){}
                Decoder(const std::string &der);
                void reset(const std::string &der);

                //We don't know how to construct a general object, the object should construct itself
                template <typename T>
                bool decode(T &res)
                {
                    auto restoreRange = range_;
                    if (!res.construct(*this))
                    {
                        //If construction failed, we restore the original range
                        range_ = restoreRange;
                        return false;
                    }
                    return true;
                }
                template <typename T>
                bool decode(std::vector<T> &res)
                {
                    ValueInfo vi;
                    if (!decompose_(vi, range_))
                        return false;
                    //Check that we have a sequence
                    if (0x10 != vi.tag)
                        return false;
                    //Fill res with the values, if they are ints
                    res.clear();
                    {
                        auto subdecoder = createSubDecoder_(vi.content);
                        while (!subdecoder.empty())
                        {
                            T t;
                            if (!subdecoder.decode(t))
                                return false;
                            res.push_back(t);
                        }
                    }
                    //We successfuly read an integer, proceed range_ to the end of the content
                    proceedToEnd_(range_, vi.content.end());
                    return true;
                }

                bool empty() const {return range_.empty();}

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

                //Creates a decoder which will work on the _same_ der_
                Decoder createSubDecoder_(Range &subRange) const;
                //Make sure that the range is inside der_
                void setRange_(Range &range){range_ = range;}

                std::shared_ptr<std::string> der_;
                Range range_;
        };
        //Reads an asn.1 Integer
        template <>
            bool Decoder::decode<int>(int &);
        //Reads both an asn.1 OctetString or an IA5String
        template <>
            bool Decoder::decode<std::string>(std::string &);
        //Reads a subdecoder, e.g., for a sequence
        template <>
            bool Decoder::decode<Decoder>(Decoder &subDecoder);
    }
}

#endif
