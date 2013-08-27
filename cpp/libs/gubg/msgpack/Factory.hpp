#ifndef HEADER_gubg_msgpack_Factory_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Factory_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Parser.hpp"
#include "gubg/msgpack/Write.hpp"
#include "gubg/msgpack/Deserialize.hpp"
#include "gubg/StateMachine.hpp"
#include "gubg/FixedVector.hpp"

#define GUBG_MODULE_ "msgpack_Factory"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace msgpack
    {
        template <typename Receiver, typename String, size_t MaxDepth>
            class Factory_crtp: public Parser_crtp<Factory_crtp<Receiver, String, MaxDepth>, FixedVector<Element, MaxDepth*2>>
            {
                    private:
                        typedef Factory_crtp<Receiver, String, MaxDepth> Self;
                        //We take twice MaxDepth, msgpack encoding is +- twice as deep as the object stack
                        typedef FixedVector<Element, MaxDepth*2> Path;
                        typedef Parser_crtp<Factory_crtp<Receiver, String, MaxDepth>, Path> Parser;
                        DATA_EVENT(Reset);
                        DATA_EVENT(OpenElement, const Element &, el, const Path &, path);

                    public:
                        typedef msgpack::TypeId TypeId;
                        typedef msgpack::AttributeId AttributeId;
                        typedef msgpack::Object_itf Object_itf;

                        Factory_crtp():
                            sm_(*this){}

                        template <typename Buffer, typename T>
                            ReturnCode serialize(Buffer &buffer, const T &t)
                            {
                                MSS_BEGIN(ReturnCode);
                                MSS(receiver_().factory_serialize(buffer, t));
                                MSS_END();
                            }
                        template <typename Buffer>
                            ReturnCode writeIdAndLength(Buffer &buffer, long id, size_t nr)
                            {
                                MSS_BEGIN(ReturnCode);
                                MSS(write(buffer, nr+1, MapTL_tag()));
                                MSS(write(buffer, Nil_tag()));
                                MSS(write(buffer, id));
                                MSS_END();
                            }
                        template <typename Buffer, typename T>
                            ReturnCode writeMember(Buffer &buffer, long id, const T &t)
                            {
                                MSS_BEGIN(ReturnCode);
                                MSS(write(buffer, id));
                                MSS(serialize(buffer, t));
                                MSS_END();
                            }
                        template <typename Buffer>
                            ReturnCode serialize(Buffer &buffer, long l) { return write(buffer, l); }
                        template <typename Buffer>
                            ReturnCode serialize(Buffer &buffer, const std::string &str) { return write(buffer, str); }

                        template <typename T>
                            Object<T> *wrap(T &t){return ::gubg::msgpack::wrap(t);}

                        template <typename Primitive>
                            void factory_primitive(Primitive primitive){S();L("Received a toplevel primitive");}

                        //Called by the parser, translated basically in events for the state machine
                        template <typename P>
                            void parser_open(const Element &e, const P &p)
                            {
                                S();L((int)e.type.group << " " << &e << " " << p.size());
                                sm_.process(OpenElement(e, p));
                            }
                        template <typename P>
                            void parser_add(Nil_tag nil, const P &p)
                            {
                                S();L("nil " << p.size());
                                sm_.process(nil);
                            }
                        template <typename P>
                            void parser_add(long l, const P &p)
                            {
                                S();L("long: " << l << " " << p.size());
                                sm_.process(l);
                            }
                        //We collect a raw string character by character and send it to the state machine
                        //when it is complete
                        template <typename P>
                            void parser_add(char ch, const P &p)
                            {
                                const auto &el = p.back();
                                S();L("char: '" << ch << "' " << p.size() << " " << el.ix << " " << el.length);
                                if (el.ix == 0)
                                    string_.clear();
                                string_.push_back(ch);
                                if (el.ix == el.length-1)
                                    sm_.process(string_);
                            }
                        template <typename P>
                            void parser_close(const Element &e, const P &p)
                            {
                                S();L((int)e.type.group << " " << p.size());
                                if (p.empty())
                                    sm_.process(Reset());
                            }
                    private:
                        Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                        enum class State {Idle, Primitive_detected, UT_map_detected, UT_nil_detected, UT_created, UT_member, SemanticError, MaximumRecursionReached};
                        typedef StateMachine_ftop<Self, State, State::Idle> SM;
                        friend class StateMachine_ftop<Self, State, State::Idle>;
                        SM sm_;
                        void sm_exit(State s)
                        {
                        }
                        void sm_enter(typename SM::State &s)
                        {
                            switch (s())
                            {
                                case State::Primitive_detected: s.changeTo(State::Idle); break;
                            }
                        }
                        void sm_event(typename SM::State &s, const OpenElement &oe)
                        {
                            S();
                            L("Received an element " << (int)oe.el.type.group << " " << &oe.el);
                            switch (s())
                            {
                                case State::Idle:
                                    switch (oe.el.type.group)
                                    {
                                        case Group::Integer:
                                            s.changeTo(State::Idle);
                                            break;
                                        case Group::Map:     s.changeTo(State::UT_map_detected); break;
                                        default:             s.changeTo(State::SemanticError); break;
                                    }
                                    break;
                            }
                        }
                        void sm_event(typename SM::State &s, Nil_tag nil)
                        {
                            switch (s())
                            {
                                case State::Idle:
                                    receiver_().factory_primitive(nil);
                                    return s.changeTo(State::Primitive_detected);
                                    break;
                                case State::UT_map_detected:
                                    return s.changeTo(State::UT_nil_detected);
                                    break;
                                case State::UT_member:
                                    if (!objectsStack_.empty())
                                    {
                                        auto obj = objectsStack_.back();
                                        obj->set(memberId_, nil);
                                        return s.changeTo(State::UT_created);
                                    }
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }
                        void sm_event(typename SM::State &s, long id)
                        {
                            switch (s())
                            {
                                case State::Idle:
                                    receiver_().factory_primitive(id);
                                    return s.changeTo(State::Primitive_detected);
                                    break;
                                case State::UT_nil_detected:
                                    {
                                        Object_itf *obj = receiver_().factory_createObject(id);
                                        if (!obj)
                                            break;
                                        const auto ls = objectsStack_.size();
                                        objectsStack_.push_back(obj);
                                        if (objectsStack_.size() != ls + 1)
                                            //Increase MaxDepth
                                            s.changeTo(State::MaximumRecursionReached);
                                        return s.changeTo(State::UT_created);
                                    }
                                    break;
                                case State::UT_created:
                                    memberId_ = id;
                                    return s.changeTo(State::UT_member);
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }
                        void sm_event(typename SM::State &s, const String &str)
                        {
                            S();L("Received string event");
                            switch (s())
                            {
                                case State::Idle:
                                    receiver_().factory_primitive(str);
                                    return s.changeTo(State::Primitive_detected);
                                    break;
                                case State::UT_member:
                                    if (!objectsStack_.empty())
                                    {
                                        auto obj = objectsStack_.back();
                                        obj->set(memberId_, str);
                                        return s.changeTo(State::UT_created);
                                    }
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }
                        void sm_event(typename SM::State &s, Reset)
                        {
                            s.changeTo(State::Idle);
                        }

                        long memberId_;

                        String string_;

                        typedef FixedVector<Object_itf*, MaxDepth> ObjectStack;
                        ObjectStack objectsStack_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
