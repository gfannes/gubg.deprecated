#ifndef HEADER_gubg_msgpack_Factory_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Factory_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Parser.hpp"
#include "gubg/msgpack/Write.hpp"
#include "gubg/msgpack/Deserialize.hpp"
#include "gubg/StateMachine.hpp"
#include "gubg/FixedVector.hpp"

#define GUBG_MODULE "msgpack_Factory"
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
                        DATA_EVENT(OpenElement, const Element &, el);
                        DATA_EVENT(CloseElement, const Element &, el);

                    public:
                        typedef msgpack::RoleId RoleId;
                        typedef msgpack::Wrapper<String> Wrapper_;

                        Factory_crtp():
                            sm_(*this){}

						/*
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
                            ReturnCode serialize(Buffer &buffer, const String &str) { return write(buffer, str); }
							*/

                        template <typename T>
                            Wrapper_ wrap(T &t){return ::gubg::msgpack::wrap<String>(t);}
                        template <typename T>
                            Wrapper_ wrapWithoutClear(T &t){return ::gubg::msgpack::wrapWithoutClear<String>(t);}

                        //Called by the parser, we translate them into events for our state machine
                        template <typename P>
                            void parser_open(const Element &e, const P &p)
                            {
                                //S();L((int)e.type.group << " " << &e << " " << p.size());
                                if (e.type.group == Group::Map)
                                    sm_.process(OpenElement(e));
                            }
                        template <typename P>
                            void parser_close(const Element &e, const P &p)
                            {
                                //S();L((int)e.type.group << " " << p.size());
                                if (e.type.group == Group::Map)
                                    sm_.process(CloseElement(e));
                            }
                        template <typename P>
                            void parser_add(Nil_tag nil, const P &p)
                            {
                                //S();L("nil");
                                sm_.process(nil);
                            }
                        template <typename P>
                            void parser_add(long l, const P &p)
                            {
                                //S();L("long: " << l);
                                sm_.process(l);
                            }
                        template <typename P>
                            void parser_add(Bool b, const P &p)
                            {
                                //S();L("Bool: " << (bool)b);
                                sm_.process(b);
                            }
                        //We collect a raw string character by character and send it to the state machine
                        //when it is complete
                        template <typename P>
                            void parser_add(char ch, const P &p)
                            {
                                const auto &el = p.back();
                                //S();L("char: '" << ch << "' " << p.size() << " " << el.ix << " " << el.length);
                                if (el.ix == 0)
                                    string_.clear();
                                string_.push_back(ch);
                                if (el.ix == el.length-1)
									//This was the last character, now we sent it the the state machine
                                    sm_.process(string_);
                            }

                        void reset() {sm_.process(Reset());}

                    private:
                        Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                        enum class State {Init, TopLevel, Struct_detected, RoleId_detected, ElementWasSet, SemanticError, MaximumRecursionReached};
                        typedef StateMachine_ftop<Self, State, State::Init> SM;
                        friend class StateMachine_ftop<Self, State, State::Init>;
                        RoleId roleId_;
						static const RoleId TopLevel_rid = -1;
                        SM sm_;
                        void sm_exit(State s)
                        {
                        }
                        void sm_enter(typename SM::State &s)
                        {
                            S();L("Entering state " << (int)s());
                            switch (s())
                            {
                                case State::Init:
                                    roleId_ = TopLevel_rid;
                                    objectsStack_.clear();
                                    s.changeTo(State::TopLevel);
                                    break;
                                case State::ElementWasSet:
                                    roleId_ = objectsStack_.back().rid;
                                    return s.changeTo(State::Struct_detected);
                                    break;
                                case State::SemanticError:
                                    {
                                        S();L("SemanticError occured");
                                    }
                                    break;
                            }
                        }
                        void sm_event(typename SM::State &s, const OpenElement &oe)
                        {
                            S();L("OpenElement " << (int)oe.el.type.group << " " << &oe.el);
							if (oe.el.type.group != Group::Map)
								return s.changeTo(State::SemanticError);

							Wrapper_ obj;
                            switch (s())
							{
								case State::TopLevel:
									//We use the receiver as the top-level entry point for deserialization
									assert(objectsStack_.empty());
									receiver_().msgpack_beginOfFrame();
									obj = wrapWithoutClear(receiver_());
									break;
								case State::RoleId_detected:
									assert(!objectsStack_.empty());
									objectsStack_.back().createObject(obj, roleId_);
									break;
							}
							//We always set the rid member
							obj.rid = roleId_;

							//push_back() into FixedVector can fail, we check it.
							{
								const auto ls = objectsStack_.size();
								objectsStack_.push_back(obj);
								if (objectsStack_.size() != ls + 1)
									//Increase MaxDepth
									s.changeTo(State::MaximumRecursionReached);
							}
							s.changeTo(State::Struct_detected);
						}
                        void sm_event(typename SM::State &s, const CloseElement &ce)
                        {
                            S();L("CloseElement " << (int)s() << " " << (int)ce.el.type.group << " " << &ce.el);
                            switch (s())
                            {
                                case State::Struct_detected:
                                    switch (ce.el.type.group)
                                    {
                                        case Group::Map:
                                            assert(!objectsStack_.empty());
                                            roleId_ = objectsStack_.back().rid;
                                            objectsStack_.pop_back();
											if (objectsStack_.empty())
											{
												assert(roleId_ == TopLevel_rid);
												receiver_().msgpack_endOfFrame();
                                                s.changeTo(State::TopLevel);
											}
											else
											{
												objectsStack_.back().createdObject(roleId_);
                                                s.changeTo(State::Struct_detected);
											}
                                            break;

                                        default: return s.changeTo(State::SemanticError);
                                    }
                                    break;
                            }
                        }
                        void sm_event(typename SM::State &s, long id)
                        {
                            S();L("long: " << id << " state: " << (int)s());
                            switch (s())
                            {
                                case State::TopLevel:
									//We received a long directly
                                    assert(objectsStack_.size() == 1);
									assert(roleId_ == TopLevel_rid);
                                    objectsStack_.back().set(roleId_, id);
                                    return;
                                    break;
                                case State::RoleId_detected:
   									//We received a long inside a struct
                                    assert(objectsStack_.size() > 1);
                                    objectsStack_.back().set(roleId_, id);
                                    return s.changeTo(State::ElementWasSet);
                                    break;
                                case State::Struct_detected:
                                    roleId_ = id;
                                    return s.changeTo(State::RoleId_detected);
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }
                        void sm_event(typename SM::State &s, Nil_tag nil)
                        {
                            S();L("nil");
                            switch (s())
                            {
                                case State::TopLevel:
									//We received a nil directly
                                    assert(objectsStack_.size() == 1);
									assert(roleId_ == TopLevel_rid);
                                    objectsStack_.back().set(roleId_, nil);
                                    return;
                                    break;
                                case State::RoleId_detected:
   									//We received a nil inside a struct
                                    assert(objectsStack_.size() > 1);
                                    objectsStack_.back().set(roleId_, nil);
                                    return s.changeTo(State::ElementWasSet);
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }
                        void sm_event(typename SM::State &s, Bool b)
                        {
                            S();L("Bool " << (bool)b);
                            switch (s())
                            {
                                case State::TopLevel:
									//We received a Bool directly
                                    assert(objectsStack_.size() == 1);
									assert(roleId_ == TopLevel_rid);
                                    objectsStack_.back().set(roleId_, b);
                                    return;
                                    break;
                                case State::RoleId_detected:
   									//We received a Bool inside a struct
                                    assert(objectsStack_.size() > 1);
                                    objectsStack_.back().set(roleId_, b);
                                    return s.changeTo(State::ElementWasSet);
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }
                        void sm_event(typename SM::State &s, const String &str)
                        {
                            S();L("String: " << str << " state: " << (int)s());
                            switch (s())
                            {
                                case State::TopLevel:
									//We received a string directly
                                    assert(objectsStack_.size() == 1);
									assert(roleId_ == TopLevel_rid);
                                    objectsStack_.back().set(roleId_, str);
                                    return;
                                    break;
                                case State::RoleId_detected:
   									//We received a string inside a struct
                                    assert(objectsStack_.size() > 1);
                                    objectsStack_.back().set(roleId_, str);
                                    return s.changeTo(State::ElementWasSet);
                                    break;
                            }
                            s.changeTo(State::SemanticError);
                        }
                        void sm_event(typename SM::State &s, Reset)
                        {
                            S();L("Reset");
                            s.changeTo(State::Init);
                        }

                        String string_;

                        typedef FixedVector<Wrapper<String>, MaxDepth> ObjectStack;
                        ObjectStack objectsStack_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
