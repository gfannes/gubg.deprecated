#ifndef gubg_msgpack_Read_hpp
#define gubg_msgpack_Read_hpp

#include "gubg/msgpack/Primitives.hpp"
#include "gubg/msgpack/Codes.hpp"

namespace gubg
{
    namespace msgpack
    {
        template <typename String>
            class Range
            {
                public:
                    Range(const String &str):
                        it_(str.begin()), end_(str.end()){}

                    ReturnCode read(Type &type)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(it_ != end_, RangeIsEmpty);
                        MSS(type.read(*(it_++)));
                        MSS_END();
                    }

                private:
                    typedef typename String::const_iterator const_iterator;
                    const_iterator it_;
                    const const_iterator end_;
            };

        template <typename String, typename Integer>
            ReturnCode read(Integer &v, const String &str, SInteger_tag)
            {
                MSS_BEGIN(ReturnCode);
                Range<String> range(str);
                Type type;
                MSS(range.read(type));
                switch (type.primitive)
                {
                    case Primitive::PosFix:
                        v = type.nr;
                        break;
                    case Primitive::NegFix:
                        v = type.nr-32;
                        break;
                    default:
                        MSS_L(ReadError);
                        break;
                }
                MSS_END();
            }
        template <typename String>
            ReturnCode read(int &v, const String &str) { return read(v, str, SInteger_tag()); }
    }
}

#endif