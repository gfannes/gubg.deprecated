#ifndef HEADER_gubg_data_Block_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_Block_hpp_ALREADY_INCLUDED

#include "gubg/tuple/TypeTransform.hpp"
#include "gubg/tuple/ForEach.hpp"
#include "gubg/tmp/IntType.hpp"
#include "gubg/data/Codes.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Strange.hpp"
#include <tuple>
#include <vector>
#include <ostream>
#include <cassert>

#define GUBG_MODULE "Block"
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

        template <typename Fields, typename Delimiter>
            struct LoadFromStrange
            {
                Fields &fields;
                const size_t record_ix;
                Strange line;
                const Delimiter &delimiter;
                bool ok = true;
                std::string msg;
                Strange tmp_value_;
                LoadFromStrange(Fields &fields, size_t record_ix, Strange line, const Delimiter &delimiter): fields(fields), record_ix(record_ix), line(line), delimiter(delimiter) { }
                template <typename FieldIX>
                    void operator()(FieldIX)
                    {
                        if (!ok)
                            return;
                        constexpr auto field_ix = FieldIX::value;
                        if (field_ix == std::tuple_size<Fields>::value-1)
                            line.popAll(tmp_value_);
                        else
                        {
                            if (!line.popUntil(tmp_value_, delimiter))
                            {
                                ok = false;
                                msg = std::string("No value found for field_ix ")+std::to_string(field_ix)+" for record_ix "+std::to_string(record_ix);
                                return;
                            }
                        }
                        if (!parse_and_set_(std::get<field_ix>(fields)[record_ix]))
                            ok = false;
                    }
                bool parse_and_set_(int &v)
                {
                    if (!tmp_value_.popDecimal(v))
                    {
                        msg = std::string("This is not an int: ")+tmp_value_.str();
                        return false;
                    }
                    return true;
                }
                bool parse_and_set_(double &v)
                {
                    if (!tmp_value_.popFloat(v))
                    {
                        msg = std::string("This is not a float: ")+tmp_value_.str();
                        return false;
                    }
                    return true;
                }
                bool parse_and_set_(std::string &v)
                {
                    v = tmp_value_.str();
                    return true;
                }
            };

        template <typename Fields, typename Record>
            struct PushBackRecord
            {
                Fields &fields;
                const Record &record;
                PushBackRecord(Fields &fields, const Record &record): fields(fields), record(record) { }
                template <typename FieldIX>
                    void operator()(FieldIX)
                    {
                        constexpr auto field_ix = FieldIX::value;
                        std::get<field_ix>(fields).push_back(std::get<field_ix>(record));
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
                typedef Block<Types...> Self;

                class const_iterator
                {
                    public:
                        bool operator==(const const_iterator &rhs) const
                        {
                            assert(self_ == rhs.self_);
                            return ix_ == rhs.ix_;
                        }
                        bool operator!=(const const_iterator &rhs) const {return !operator==(rhs);}
                        Record operator*() const
                        {
                            assert((bool)self_);
                            return self_->record(ix_);
                        }
                        const_iterator &operator++()
                        {
                            assert((bool)self_);
                            ++ix_;
                            return *this;
                        }
                        const_iterator &operator--()
                        {
                            assert((bool)self_);
                            --ix_;
                            return *this;
                        }
                        const_iterator operator+(size_t d) const
                        {
                            assert((bool)self_);
                            auto it = *this;
                            it.ix_ += d;
                            return it;
                        }
                    private:
                        friend class Block<Types...>;
                        size_t ix_;
                        const Self *self_ = nullptr;
                };

                Block(){}
                Block(size_t nr)
                {
                    resize(nr);
                }

                const_iterator begin() const
                {
                    const_iterator it;
                    it.ix_ = 0;
                    it.self_ = this;
                    return it;
                }
                const_iterator end() const
                {
                    const_iterator it;
                    it.ix_ = size();
                    it.self_ = this;
                    return it;
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

                template <typename Delimiter>
                ReturnCode load(const gubg::file::File &fn, const Delimiter &delimiter)
                {
                    MSS_BEGIN(ReturnCode);
                    std::string content;
                    MSS(file::read(content, fn));
                    Strange line;
                    //Count the number of lines and do a resize
                    {
                        Strange strange(content);
                        size_t nr_lines = 0;
                        while (strange.popLine(line)) { ++nr_lines; }
                        resize(nr_lines);
                    }
                    Strange strange(content);
                    Strange value;
                    const auto s = size();
                    for (size_t ix = 0; ix < s; ++ix)
                    {
                        const bool ok = strange.popLine(line);
                        assert(ok);//Should not fail, we have done the resize() ourselves
                        details::LoadFromStrange<Fields, Delimiter> loadFromStrange(fields_, ix, line, delimiter);
                        tuple::for_each(field_ixs_, loadFromStrange);
                        if (!loadFromStrange.ok)
                        {
                            L(loadFromStrange.msg);
                            MSS(loadFromStrange.ok);
                        }
                    }
                    assert(!strange.popLine(line));
                    MSS_END();
                }

                void push_back(const Record &record)
                {
                    details::PushBackRecord<Fields, Record> pushBackRecord(fields_, record);
                    tuple::for_each(field_ixs_, pushBackRecord);
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
