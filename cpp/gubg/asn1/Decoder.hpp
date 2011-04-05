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
        //DER-specific decoder
        class Decoder
        {
            public:
                Decoder();
                Decoder(const std::string &der);
                virtual ~Decoder();

                void reset(const std::string &der);

                //We extracting from a decoder, it is always checked that we are not blocked by a subdecoder
                //If this is the case, a Decoder::Blocked exception will be thrown
                struct Blocked: Exception
                {
                    Blocked(): Exception("Decoder is blocked"){}
                };

                //We don't know how to construct a general object, the object should construct itself
                template <typename T>
                bool extract(T &res)
                {
                    checkNotBlocked_();
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
                bool extract(std::vector<T> &res)
                {
                    checkNotBlocked_();
                    ValueInfo vi;
                    if (!decompose_(vi, range_))
                        return false;
                    //Check that we have a sequence
                    if (0x10 != vi.tag)
                        return false;
                    //Fill res with the values, if they are ints
                    res.clear();
                    {
                        Decoder subdecoder;
                        createSubDecoder_(subdecoder, vi.content);
                        while (!subdecoder.empty())
                        {
                            T t;
                            if (!subdecoder.extract(t))
                                return false;
                            res.push_back(t);
                        }
                    }
                    //We successfuly read an integer, proceed range_ to the end of the content
                    proceedToEnd_(range_, vi.content.end());
                    return true;
                }

                bool empty() const {return range_.empty();}

                void indicateDecodingOK(){decodingOK_ = true;}

            private:
                Decoder &operator=(const Decoder &);

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
                void createSubDecoder_(Decoder &subdecoder, Range &subRange);

                std::shared_ptr<std::string> der_;
                Range range_;

                //A subdecoder is always created from a parent_.
                //The parent_ will be blocked until the subdecoder is destructed
                //When the subdecoder destructs, it will unblock its parent decoder
                Decoder *parent_;
                bool isBlocked_;
                //The method check that we are _not_ blocked. If we are, it will throw a Decoder::Blocked exception
                void checkNotBlocked_() const throw (Decoder::Blocked);
                //If a subdecoder could be read correctly, this will be set to true via indicateDecodingOK()
                //As such, its dtor will advance the range_ of the parent
                bool decodingOK_;
        };
        //Reads an asn.1 Integer
        template <>
            bool Decoder::extract<int>(int &);
        //Reads both an asn.1 OctetString or an IA5String
        template <>
            bool Decoder::extract<std::string>(std::string &);
        //Reads a subdecoder, e.g., for a sequence
        template <>
            bool Decoder::extract<Decoder>(Decoder &subdecoder);
    }
}

#endif
