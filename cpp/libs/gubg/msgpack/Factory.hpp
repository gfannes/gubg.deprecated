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
                                MSS_END();
                            }
                    private:
                        Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                        enum class State {Idle, Map, CreateObj, CreatedObj, SemanticError};
                        typedef StateMachine_ftop<Self, State, State::Idle> SM;
                        friend class StateMachine_ftop<Self, State, State::Idle>;
                        SM sm_;
                        void sm_exit(State s)
                        {
                        }
                        void sm_enter(typename SM::State &s)
                        {
                        }
                        void sm_event(typename SM::State &s, const Element &el)
                        {
                            switch (s())
                            {
                                case State::Idle:
                                    if (el.type.group == Group::Map)
                                        s.changeTo(State::Map);
                                    else
                                        s.changeTo(State::SemanticError);
                                    break;
                            }
                        }
                        void sm_event(typename SM::State &s, Nil_tag)
                        {
                            switch (s())
                            {
                                case State::Map:
                                    s.changeTo(State::CreateObj);
                                    break;
                            }
                        }
                        void sm_event(typename SM::State &s, unsigned long id)
                        {
                            switch (s())
                            {
                                case State::CreateObj:
                                    {
                                        IObject *obj = receiver_().factory_createObject(id);
                                        if (!obj)
                                            break;
                                        s.changeTo(State::CreatedObj);
                                        return;
                                    }
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }

                        typedef std::queue<IObject*> Objects;
                        Objects objects_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
