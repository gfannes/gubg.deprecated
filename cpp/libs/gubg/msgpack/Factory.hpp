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
                            private:
                        };
                        template <typename T>
                            class Object: public IObject
                        {
                        };

                        template <typename P>
                            ReturnCode parser_open(const Element &e, const P &p)
                            {
                                MSS_BEGIN(ReturnCode, (int)e.type.group << " " << p.size());
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

                        enum class State {Idle, Primitive_detected, UT_map_detected, UT_nil_detected, UT_created, SemanticError};
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
                                case State::Idle: Parser::clear(); break;
                                case State::Primitive_detected: s.changeTo(State::Idle); break;
                            }
                        }
                        void sm_event(typename SM::State &s, const Element &el)
                        {
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
                        void sm_event(typename SM::State &s, Nil_tag)
                        {
                            switch (s())
                            {
                                case State::UT_map_detected:
                                    s.changeTo(State::UT_nil_detected);
                                    break;
                            }
                        }
                        void sm_event(typename SM::State &s, unsigned long id)
                        {
                            switch (s())
                            {
                                case State::Idle:
                                    receiver_().factory_primitive(id);
                                    s.changeTo(State::Primitive_detected);
                                    return;
                                    break;
                                case State::UT_nil_detected:
                                    {
                                        IObject *obj = receiver_().factory_createObject(id);
                                        if (!obj)
                                            break;
                                        s.changeTo(State::UT_created);
                                        return;
                                    }
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }
                        void sm_event(typename SM::State &s, Reset)
                        {
                            s.changeTo(State::Idle);
                        }

                        typedef std::queue<IObject*> Objects;
                        Objects objects_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
