#ifndef HEADER_gubg_msgpack_Factory_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Factory_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Parser.hpp"
#include "gubg/StateMachine.hpp"
#include "gubg/FixedVector.hpp"
#include <queue>

#define GUBG_MODULE_ "msgpack_Factory"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace msgpack
    {
        template <typename Receiver, size_t MaxString>
            class Factory_crtp: public Parser_crtp<Factory_crtp<Receiver, MaxString>, std::vector<Element>>
            {
                    private:
                        typedef Factory_crtp<Receiver, MaxString> Self;
                        typedef Parser_crtp<Factory_crtp<Receiver, MaxString>, std::vector<Element>> Parser;
                        typedef std::vector<Element> Path;
                        DATA_EVENT(Reset);
                        DATA_EVENT(OpenElement, const Element &, el, const Path &, path);

                    public:
                        Factory_crtp():
                            sm_(*this){}

                        class IObject
                        {
                            public:
                                virtual void set(Receiver &receiver, long id, Nil_tag) = 0;
                            private:
                        };
                        template <typename T>
                            class Object: public IObject
                        {
                            public:
                                Object(T &t):obj_(t){}
                                virtual void set(Receiver &receiver, long id, Nil_tag nil)
                                {
                                    receiver.factory_setMember(obj_, id, nil);
                                }
                            private:
                                T &obj_;
                        };
                        template <typename T>
                            Object<T> *wrap(T &t){return new Object<T>(t);}

                        template <typename Primitive>
                            void factory_primitive(Primitive primitive){S();L("Received a toplevel primitive");}

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
                        template <typename P>
                            void parser_add(char ch, const P &p)
                            {
                                S();L("char: " << ch << " " << p.size());
                                sm_.process(ch);
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

                        enum class State {Idle, Primitive_detected, UT_map_detected, UT_nil_detected, UT_created, UT_member, SemanticError};
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
                                    if (!objects_.empty())
                                    {
                                        auto obj = objects_.back();
                                        obj->set(receiver_(), memberId_, nil);
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
                                        IObject *obj = receiver_().factory_createObject(id);
                                        if (!obj)
                                            break;
                                        objects_.push(obj);
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
                        void sm_event(typename SM::State &s, Reset)
                        {
                            s.changeTo(State::Idle);
                        }

                        long memberId_;

                        typedef FixedVector<char, MaxString> String;
                        String string_;

                        typedef std::queue<IObject*> Objects;
                        Objects objects_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
