#ifndef HEADER_gubg_data_Block_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Block_hpp_ALREADY_INCLUDED

#include "gubg/tuple/TypeTransform.hpp"
#include "gubg/tuple/ForEach.hpp"
#include "gubg/tmp/IntType.hpp"
#include <tuple>
#include <vector>
#include <ostream>
#include <cassert>

namespace gubg { namespace data { 

    namespace details { 

        template <typename T, size_t IX>
            struct FieldTransform { typedef std::vector<T> Type; };
        //Can be used with tuple::for_each to go over the IXs at compile time
        template <typename T, size_t IX>
            struct IXTransform { typedef gubg::tmp::IntType<IX> Type; };

        template <size_t NrFields>
            struct GetSize
            {
                template <typename Tuple>
                    static size_t size(const Tuple &t) {return std::get<0>(t).size();}
            };
        template <>
            struct GetSize<0>
            {
                template <typename Tuple>
                    static size_t size(const Tuple &) {return 0;}
            };

        struct DoResize
        {
            const size_t nr;
            DoResize(size_t nr): nr(nr) {}
            template <typename Field>
                void operator()(Field &field) const
                {
                    field.resize(nr);
                }
        };

        template <typename Record, typename Fields>
            struct GetRecord
            {
                Record &record;
                const Fields &fields;
                const size_t record_ix;
                GetRecord(Record &record, const Fields &fields, size_t record_ix): record(record), fields(fields), record_ix(record_ix) {}
                template <typename IX>
                    void operator()(IX) const
                    {
                        constexpr auto field_ix = IX::value;
                        std::get<field_ix>(record) = std::get<field_ix>(fields)[record_ix];
                    }
            };


        struct PrintValue
        {
            std::ostream &os;
            PrintValue(std::ostream &os): os(os) {}
            template <typename V, typename IX>
                void operator()(const V &v, IX) const
                {
                    if (IX::value > 0)
                        os << ", ";
                    os << v;
                }
        };
        struct PrintRecord
        {
            std::ostream &os;
            PrintValue pv;
            PrintRecord(std::ostream &os): os(os), pv(os) {}
            template <typename Record>
                void operator()(const Record &r) const
                {
                    tuple::for_each_with_ix(r, pv);
                    os << std::endl;
                }
        };
    } 

    template <typename... Types>
        class Block
        {
            public:
                typedef std::tuple<Types...> Record;

                size_t size() const
                {
                    return details::GetSize<std::tuple_size<Fields>::value>::size(fields_);
                }

                void resize(size_t nr)
                {
                    details::DoResize do_resize(nr);
                    tuple::for_each(fields_, do_resize);
                }

                void stream(std::ostream &os) const
                {
                    each_record(details::PrintRecord(os));
                }

                Record record(size_t ix) const
                {
                    assert(ix < size());
                    Record r;
                    details::GetRecord<Record, Fields> get_record(r, fields_, ix);
                    tuple::for_each(field_ixs_, get_record);
                    return r;
                }

                template <typename Ftor>
                    void each_record(const Ftor &ftor) const
                    {
                        const auto s = size();
                        for (size_t ix = 0; ix < s; ++ix)
                            ftor(record(ix));
                    }

            private:
                typedef typename tuple::TypeTransform<Record, details::FieldTransform>::Type Fields;
                Fields fields_;
                typedef typename tuple::TypeTransform<Record, details::IXTransform>::Type FieldIXs;
                static const FieldIXs field_ixs_;
        };
    template <typename... Types>
        const typename Block<Types...>::FieldIXs Block<Types...>::field_ixs_;

    template <typename... Types>
        std::ostream &operator<<(std::ostream &os, const Block<Types...> &block)
        {
            block.stream(os);
            return os;
        }

} } 

#endif
