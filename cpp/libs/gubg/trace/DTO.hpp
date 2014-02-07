#ifndef HEADER_gubg_trace_DTO_hpp_ALREADY_INCLUDED
#define HEADER_gubg_trace_DTO_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Deserialize.hpp"
#include <ostream>

#define GUBG_MODULE_ "DTO"
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

        enum {pid_, tid_, tp_ms_, category_, msg_, nr_};
        template <typename S>
            bool msgpack_serialize(S &s) const
            {
                s.writeIdAndAttrCnt(S::OpenScope, nr_);
                s.template writeAttribute<long>(pid_, pid);
                s.template writeAttribute<long>(tid_, tid);
                s.template writeAttribute<long>(tp_ms_, tp_ms);
                s.template writeAttribute<long>(category_, category);
                s.template writeAttribute<std::string>(msg_, msg);
                return true;
            }
        void msgpack_set(gubg::msgpack::AttributeId aid, gubg::msgpack::Nil_tag) {}
        template <typename String>
            void msgpack_set(gubg::msgpack::AttributeId aid, const String &str)
            {
                S();L(STREAM(aid, str));
                switch (aid)
                {
                    case msg_: msg = str; break;
                }
            }
        void msgpack_set(gubg::msgpack::AttributeId aid, long v)
        {
            S();L(STREAM(aid, v));
            switch (aid)
            {
                case pid_: pid = v; break;
                case tid_: tid = v; break;
                case tp_ms_: tp_ms = v; break;
                case category_: category = v; break;
            }
        }
    };
	std::ostream &operator<<(std::ostream &os, const OpenScope &obj)
	{
		os << STREAM(obj.pid, obj.tid, obj.tp_ms, obj.category, obj.msg);
		return os;
	}
    struct CloseScope: Origin
    {
        enum {pid_, tid_, tp_ms_, nr_};
        template <typename S>
            bool msgpack_serialize(S &s) const
            {
                s.writeIdAndAttrCnt(S::CloseScope, nr_);
                s.template writeAttribute<long>(pid_, pid);
                s.template writeAttribute<long>(tid_, tid);
                s.template writeAttribute<long>(tp_ms_, tp_ms);
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
                case pid_: pid = v; break;
                case tid_: tid = v; break;
                case tp_ms_: tp_ms = v; break;
            }
        }
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

        enum {pid_, tid_, tp_ms_, category_, msg_, nr_};
        template <typename S>
            bool msgpack_serialize(S &s) const
            {
                if (isOpen)
                {
                    s.writeIdAndAttrCnt(S::OpenScope, nr_);
                    s.template writeAttribute<long>(pid_, pid);
                    s.template writeAttribute<long>(tid_, tid);
                    s.template writeAttribute<long>(tp_ms_, tp_ms);
                    s.template writeAttribute<long>(category_, category);
                    s.template writeAttribute<std::string>(msg_, msg);
                }
                else
                {
                    s.writeIdAndAttrCnt(S::CloseScope, category_);
                    s.template writeAttribute<long>(pid_, pid);
                    s.template writeAttribute<long>(tid_, tid);
                    s.template writeAttribute<long>(tp_ms_, tp_ms);
                }
                return true;
            }
    };

} } } 
#include "gubg/log/end.hpp"
#endif
