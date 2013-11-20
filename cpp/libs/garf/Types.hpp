#ifndef HEADER_garf_Types_hpp_ALREADY_INCLUDED
#define HEADER_garf_Types_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Deserialize.hpp"

#define GUBG_MODULE_ "garf_Types"
#include "gubg/log/begin.hpp"
namespace garf
{
    namespace pod
    {
        struct TypeIds
        {
            enum {Time, TopInfo, Led, Motor};
        };

        struct Time
        {
            Time(): millis(0) {}
            unsigned long millis;

            enum {millis_, nr_};
            template <typename S>
                bool msgpack_serialize(S &s) const
                {
                    s.writeIdAndAttrCnt(S::Time, nr_);
                    s.template writeAttribute<long>(millis_, millis);
                    return true;
                }
            void msgpack_set(gubg::msgpack::AttributeId aid, gubg::msgpack::Nil_tag) {}
            template <typename String>
                void msgpack_set(gubg::msgpack::AttributeId aid, const String &str) {}
            void msgpack_set(gubg::msgpack::AttributeId aid, long v)
            {
                S();L(STREAM(aid, v));
                switch (aid)
                {
                    case millis_: millis = v; break;
                }
            }
        };

        struct TopInfo
        {
            unsigned long nrLoops;
            unsigned long maxElapse;

            TopInfo():nrLoops(0), maxElapse(0){}

            enum {nrLoops_, maxElapse_, nr_};
            template <typename S>
                bool msgpack_serialize(S &s) const
                {
                    s.writeIdAndAttrCnt(S::TopInfo, nr_);
                    s.template writeAttribute<long>(nrLoops_, nrLoops);
                    s.template writeAttribute<long>(maxElapse_, maxElapse);
                    return true;
                }
            void msgpack_set(gubg::msgpack::AttributeId aid, gubg::msgpack::Nil_tag) {}
            template <typename String>
                void msgpack_set(gubg::msgpack::AttributeId aid, const String &str) {}
            void msgpack_set(gubg::msgpack::AttributeId aid, long v)
            {
                S();L(STREAM(aid, v));
                switch (aid)
                {
                    case nrLoops_: nrLoops = v; break;
                    case maxElapse_: maxElapse = v; break;
                }
            }
        };

        struct Led
        {
            unsigned char id;
            uint8_t pattern;

            enum Attribute {id_, pattern_, nr_};
            template <typename S>
                bool msgpack_serialize(S &s) const
                {
                    s.writeIdAndAttrCnt(S::Led, nr_);
                    s.template writeAttribute<long>(id_, id);
                    s.template writeAttribute<long>(pattern_, pattern);
                    return true;
                }
            void msgpack_set(gubg::msgpack::AttributeId aid, gubg::msgpack::Nil_tag) {}
            template <typename String>
                void msgpack_set(gubg::msgpack::AttributeId aid, const String &str) {}
            void msgpack_set(gubg::msgpack::AttributeId aid, long v)
            {
                switch (aid)
                {
                    case id_: id = v; break;
                    case pattern_: pattern = v; break;
                }
            }
        };

        struct Motor
        {
            int left;
            int right;

            enum Attribute {left_, right_, nr_};
            template <typename S>
                bool msgpack_serialize(S &s) const
                {
                    s.writeIdAndAttrCnt(S::Motor, nr_);
                    s.template writeAttribute<long>(left_, left);
                    s.template writeAttribute<long>(right_, right);
                    return true;
                }
            void msgpack_set(gubg::msgpack::AttributeId aid, gubg::msgpack::Nil_tag) {}
            template <typename String>
                void msgpack_set(gubg::msgpack::AttributeId aid, const String &str) {}
            void msgpack_set(gubg::msgpack::AttributeId aid, long v)
            {
                switch (aid)
                {
                    case left_: left = v; break;
                    case right_: right = v; break;
                }
            }
        };
    }
}
#include "gubg/log/end.hpp"

#endif
