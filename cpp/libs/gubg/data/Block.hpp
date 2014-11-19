#ifndef HEADER_gubg_data_Block_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Block_hpp_ALREADY_INCLUDED

#include "gubg/tuple/TypeTransform.hpp"
#include "gubg/tuple/ForEach.hpp"
#include "gubg/tmp/IntType.hpp"
#include <tuple>
#include <vector>
#include <ostream>
#include <cassert>

#define GUBG_MODULE_ "Block"
#include "gubg/log/begin.hpp"
namespace gubg { namespace data { 

    namespace details { 

        template <typename T, size_t FieldIX>
            struct FieldTransform { typedef std::vector<T> Type; };
        //Can be used with tuple::for_each to go over the IXs at compile time
        template <typename T, size_t FieldIX>
            struct IXTransform { typedef gubg::tmp::IntType<FieldIX> Type; };

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
                template <typename FieldIX>
                    void operator()(FieldIX) const
                    {
                        constexpr auto field_ix = FieldIX::value;
                        std::get<field_ix>(record) = std::get<field_ix>(fields)[record_ix];
                    }
            };

        template <typename SrcFields, typename DstFields>
            struct CopyFields
            {
                const SrcFields &src;
                DstFields &dst;
                CopyFields(const SrcFields &src, DstFields &dst): src(src), dst(dst) {}
                template <typename FieldIX>
                    void operator()(FieldIX) const
                    {
                        constexpr auto field_ix = FieldIX::value;
                        std::get<field_ix>(dst) = std::get<field_ix>(src);
                    }
            };
        template <typename SrcFields, typename DstFields>
            struct SwapFields
            {
                SrcFields &src;
                DstFields &dst;
                SwapFields(SrcFields &src, DstFields &dst): src(src), dst(dst) {}
                template <typename FieldIX>
                    void operator()(FieldIX) const
                    {
                        constexpr auto field_ix = FieldIX::value;
                        std::get<field_ix>(dst).swap(std::get<field_ix>(src));
                    }
            };

        struct PrintValue
        {
            std::ostream &os;
            PrintValue(std::ostream &os): os(os) {}
            template <typename V, typename FieldIX>
                void operator()(const V &v, FieldIX) const
                {
                    if (FieldIX::value > 0)
                        os << ", ";
                    print_(v);
                }
            template <typename V>
                void print_(const V &v) const { os << v; }
            void print_(const std::string &v) const { os << '\"' << v << '\"'; }
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

                Block(){}
                Block(size_t nr)
                {
                    resize(nr);
                }

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
                    os << std::tuple_size<Fields>::value << " fields" << std::endl;
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

                //Transforms field FieldIX with the value from ftor(record)
                template <size_t FieldIX, typename Ftor>
                    void transform(const Ftor &ftor)
                    {
                        const auto s = size();
                        for (size_t ix = 0; ix < s; ++ix)
                            std::get<FieldIX>(fields_)[ix] = ftor(record(ix));
                    }

                template <typename T, typename Ftor>
                    Block<Types..., T> expand(const Ftor &ftor) const
                    {
                        S();
                        typedef Block<Types..., T> NewBlock;
                        NewBlock newblock;

                        //Copy all fields from this to newblock
                        details::CopyFields<Fields, typename NewBlock::Fields> copyFields(fields_, newblock.fields_);
                        tuple::for_each(field_ixs_, copyFields);

                        //Create the new field into newblock
                        const auto s = size();
                        std::get<std::tuple_size<Fields>::value>(newblock.fields_).resize(s);
                        for (size_t ix = 0; ix < s; ++ix)
                            std::get<std::tuple_size<Fields>::value>(newblock.fields_)[ix] = ftor(record(ix));

                        return newblock;
                    }

                template <typename T, typename Ftor>
                    Block<Types..., T> swap_expand(const Ftor &ftor)
                    {
                        S();
                        typedef Block<Types..., T> NewBlock;
                        NewBlock newblock;

                        //Create the new field into newblock
                        const auto s = size();
                        std::get<std::tuple_size<Fields>::value>(newblock.fields_).resize(s);
                        for (size_t ix = 0; ix < s; ++ix)
                            std::get<std::tuple_size<Fields>::value>(newblock.fields_)[ix] = ftor(record(ix));

                        //Swap all fields from this to newblock
                        //Keep this _after_ the creation of the new field, else, the content of this will be gone
                        details::SwapFields<Fields, typename NewBlock::Fields> swapFields(fields_, newblock.fields_);
                        tuple::for_each(field_ixs_, swapFields);

                        return newblock;
                    }

            private:
                template <typename...>
                    friend class Block;
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
#include "gubg/log/end.hpp"

#endif
