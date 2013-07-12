#ifndef HEADER_gubg_msgpack_Factory_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Factory_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Parser.hpp"
#include "gubg/StateMachine.hpp"
#include <queue>

#define GUBG_MODULE_ "msgpack_Factory"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace msgpack
    {
        template <typename Receiver>
            class Factory_crtp: public Parser_crtp<Factory_crtp<Receiver>, std::vector<Element>>
            {
                    private:
                        typedef Factory_crtp<Receiver> Self;
                        typedef Parser_crtp<Factory_crtp<Receiver>, std::vector<Element>> Parser;
                        DATA_EVENT(Reset);

                    public:
                        Factory_crtp():
                            sm_(*this){}

                        class IObject
                        {
                            public:
                                virtual void set(Receiver &receiver, unsigned long id, Nil_tag) = 0;
                            private:
                        };
                        template <typename T>
                            class Object: public IObject
                        {
                            public:
                                Object(T &t):obj_(t){}
                                virtual void set(Receiver &receiver, unsigned long id, Nil_tag nil)
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
                            ReturnCode parser_open(const Element &e, const P &p)
                            {
                                MSS_BEGIN(ReturnCode, (int)e.type.group << " " << &e << " " << p.size());
                                sm_.process(e);
                                MSS_END();
                            }
                        template <typename P>
                            ReturnCode parser_add(Nil_tag nil, const P &p)
                            {
                                MSS_BEGIN(ReturnCode, "nil " << p.size());
                                sm_.process(nil);
                                MSS_END();
                            }
                        template <typename P>
                            ReturnCode parser_add(unsigned long l, const P &p)
                            {
                                MSS_BEGIN(ReturnCode, l << " " << p.size());
                                sm_.process(l);
                                MSS_END();
                            }
                        template <typename P>
                            ReturnCode parser_close(const Element &e, const P &p)
                            {
                                MSS_BEGIN(ReturnCode, (int)e.type.primitive << " " << p.size());
                                if (p.empty())
                                    sm_.process(Reset());
                                MSS_END();
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
                        void sm_event(typename SM::State &s, const Element &el)
                        {
                            S();
                            L("Received an element " << (int)el.type.group << " " << &el);
                            switch (s())
                            {
                                case State::Idle:
                                    switch (el.type.group)
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
                        void sm_event(typename SM::State &s, unsigned long id)
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

                        unsigned long memberId_;

                        typedef std::queue<IObject*> Objects;
                        Objects objects_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
