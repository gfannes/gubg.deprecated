#ifndef HEADER_gubg_trace_DTO_hpp_ALREADY_INCLUDED
#define HEADER_gubg_trace_DTO_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Deserialize.hpp"
#include <ostream>

#define GUBG_MODULE "DTO"
#include "gubg/log/begin.hpp"
namespace gubg { namespace trace { namespace dto { 

    struct TypeIds
    {
        enum {OpenScope, CloseScope};
    };

    struct Origin
    {
        unsigned long pid;
        unsigned long tid;
        unsigned long tp_ms;
    };

    struct OpenScope: Origin
    {
        unsigned long category;
        std::string msg;

        enum {pid_rid, tid_rid, tp_ms_rid, category_rid, msg_rid, nr_};
        template <typename Serializer>
            bool msgpack_serialize(Serializer &s) const
            {
                auto c = s.createComposer(nr_);
                if (!c.ok()) return false;
                if (!c.put(pid_rid, pid)) return false;
                if (!c.put(tid_rid, tid)) return false;
                if (!c.put(tp_ms_rid, tp_ms)) return false;
                if (!c.put(category_rid, category)) return false;
                if (!c.put(msg_rid, msg)) return false;
                if (!c.full()) return false;
                return true;
            }

        template <typename Wrapper>
            void msgpack_createObject(Wrapper &obj, gubg::msgpack::RoleId rid) {}
        void msgpack_set(gubg::msgpack::RoleId rid, gubg::msgpack::Nil_tag) {}
        template <typename String>
            void msgpack_set(gubg::msgpack::RoleId rid, const String &str)
            {
                S();L(STREAM(rid, str));
                switch (rid)
                {
                    case msg_rid: msg = str; break;
                }
            }
        void msgpack_set(gubg::msgpack::RoleId rid, long v)
        {
            S();L(STREAM(rid, v));
            switch (rid)
            {
                case pid_rid: pid = v; break;
                case tid_rid: tid = v; break;
                case tp_ms_rid: tp_ms = v; break;
                case category_rid: category = v; break;
            }
        }
        void msgpack_createdObject(gubg::msgpack::RoleId rid) {}
    };
    std::ostream &operator<<(std::ostream &os, const OpenScope &obj)
    {
        os << STREAM(obj.pid, obj.tid, obj.tp_ms, obj.category, obj.msg);
        return os;
    }

    struct CloseScope: Origin
    {
        enum {pid_rid, tid_rid, tp_ms_rid, nr_};
        template <typename Serializer>
            bool msgpack_serialize(Serializer &s) const
            {
                auto c = s.createComposer(nr_);
                if (!c.ok()) return false;
                if (!c.put(pid_rid, pid)) return false;
                if (!c.put(tid_rid, tid)) return false;
                if (!c.put(tp_ms_rid, tp_ms)) return false;
                if (!c.full()) return false;
                return true;
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
                case pid_rid: pid = v; break;
                case tid_rid: tid = v; break;
                case tp_ms_rid: tp_ms = v; break;
            }
        }
        void msgpack_createdObject(gubg::msgpack::RoleId rid) {}
    };
    std::ostream &operator<<(std::ostream &os, const CloseScope &obj)
    {
        os << STREAM(obj.pid, obj.tid, obj.tp_ms);
        return os;
    }

    //This is only used at the sending side and can only be serialized to msgpack
    struct Scope: Origin
    {
        bool isOpen;
        unsigned long category;
        std::string msg;

        enum {pid_rid, tid_rid, tp_ms_rid, category_rid, msg_rid, nr_};
        template <typename Serializer>
            bool msgpack_serialize(Serializer &s) const
            {
                TODO::There is a problem here, the type of open or close scope should be inserted too. test it against apps/ttt
                if (isOpen)
                {
                    auto c = s.createComposer(nr_);
                    if (!c.ok()) return false;
                    if (!c.put(pid_rid, pid)) return false;
                    if (!c.put(tid_rid, tid)) return false;
                    if (!c.put(tp_ms_rid, tp_ms)) return false;
                    if (!c.put(category_rid, category)) return false;
                    if (!c.put(msg_rid, msg)) return false;
                    if (!c.full()) return false;
                }
                else
                {
                    auto c = s.createComposer(nr_);
                    if (!c.ok()) return false;
                    if (!c.put(pid_rid, pid)) return false;
                    if (!c.put(tid_rid, tid)) return false;
                    if (!c.put(tp_ms_rid, tp_ms)) return false;
                    if (!c.full()) return false;
                }
                return true;
            }
    };

} } } 
#include "gubg/log/end.hpp"
#endif
