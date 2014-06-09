#ifndef HEADER_garf_Types_hpp_ALREADY_INCLUDED
#define HEADER_garf_Types_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Deserialize.hpp"

#define GUBG_MODULE_ "garf_Types"
#include "gubg/log/begin.hpp"
namespace garf { namespace pod {

    struct TypeIds
    {
        enum {Time, TopInfo, Led, Motor};
    };

    struct Time
    {
        enum {TypeId = TypeIds::Time};

        unsigned long millis = 0;

        enum {millis_rid, nr_};
        template <typename Serializer>
            bool msgpack_serialize(Serializer &s) const
            {
                MSS_BEGIN(bool);
                auto c = s.createComposer(nr_);
                MSS(c.ok());
                MSS(c.put(millis_rid, millis));
                MSS(c.full());
                MSS_END();
            }

        template <typename Wrapper>
            void msgpack_createObject(Wrapper &obj, gubg::msgpack::RoleId rid) {}
        void msgpack_set(gubg::msgpack::RoleId rid, gubg::msgpack::Nil_tag) {}
        template <typename String>
            void msgpack_set(gubg::msgpack::RoleId rid, const String &str) {}
        void msgpack_set(gubg::msgpack::RoleId rid, long v)
        {
            S();L(STREAM(rid, v));
            switch (rid)
            {
                case millis_rid: millis = v; break;
            }
        }
        void msgpack_createdObject(gubg::msgpack::RoleId rid) {}
    };

    struct TopInfo
    {
        enum {TypeId = TypeIds::TopInfo};

        unsigned long nrLoops = 0;
        unsigned long maxElapse = 0;

        enum {nrLoops_rid, maxElapse_rid, nr_};
        template <typename Serializer>
            bool msgpack_serialize(Serializer &s) const
            {
                MSS_BEGIN(bool);
                auto c = s.createComposer(nr_);
                MSS(c.ok());
                MSS(c.put(nrLoops_rid, nrLoops));
                MSS(c.put(maxElapse_rid, maxElapse));
                MSS(c.full());
                MSS_END();
            }

        template <typename Wrapper>
            void msgpack_createObject(Wrapper &obj, gubg::msgpack::RoleId rid) {}
        void msgpack_set(gubg::msgpack::RoleId rid, gubg::msgpack::Nil_tag) {}
        template <typename String>
            void msgpack_set(gubg::msgpack::RoleId rid, const String &str) {}
        void msgpack_set(gubg::msgpack::RoleId rid, long v)
        {
            S();L(STREAM(rid, v));
            switch (rid)
            {
                case nrLoops_rid: nrLoops = v; break;
                case maxElapse_rid: maxElapse = v; break;
            }
        }
        void msgpack_createdObject(gubg::msgpack::RoleId rid) {}
    };

    struct Led
    {
        enum {TypeId = TypeIds::Led};

        unsigned char id = 0;
        uint8_t pattern = 0;

        enum {id_rid, pattern_rid, nr_};
        template <typename Serializer>
            bool msgpack_serialize(Serializer &s) const
            {
                MSS_BEGIN(bool);
                auto c = s.createComposer(nr_);
                MSS(c.ok());
                MSS(c.put(id_rid, id));
                MSS(c.put(pattern_rid, pattern));
                MSS(c.full());
                MSS_END();
            }

        template <typename Wrapper>
            void msgpack_createObject(Wrapper &obj, gubg::msgpack::RoleId rid) {}
        void msgpack_set(gubg::msgpack::RoleId rid, gubg::msgpack::Nil_tag) {}
        template <typename String>
            void msgpack_set(gubg::msgpack::RoleId rid, const String &str) {}
        void msgpack_set(gubg::msgpack::RoleId rid, long v)
        {
            switch (rid)
            {
                case id_rid: id = v; break;
                case pattern_rid: pattern = v; break;
            }
        }
        void msgpack_createdObject(gubg::msgpack::RoleId rid) {}
    };

    struct Motor
    {
        enum {TypeId = TypeIds::Motor};

        int left = 0;
        int right = 0;

        enum {left_rid, right_rid, nr_};
        template <typename S>
            bool msgpack_serialize(S &s) const
            {
                MSS_BEGIN(bool);
                auto c = s.createComposer(2);
                MSS(c.ok());
                MSS(c.put(left_rid, left));
                MSS(c.put(right_rid, right));
                MSS(c.full());
                MSS_END();
            }

        template <typename Wrapper>
            void msgpack_createObject(Wrapper &obj, gubg::msgpack::RoleId rid) {}
        void msgpack_set(gubg::msgpack::RoleId rid, gubg::msgpack::Nil_tag) {}
        template <typename String>
            void msgpack_set(gubg::msgpack::RoleId rid, const String &str) {}
        void msgpack_set(gubg::msgpack::RoleId rid, long v)
        {
            switch (rid)
            {
                case left_rid: left = v; break;
                case right_rid: right = v; break;
            }
        }
        void msgpack_createdObject(gubg::msgpack::RoleId rid) {}
    };

} }
#include "gubg/log/end.hpp"

#endif
