#include "gubg/coding/d9/parser.hpp"
#include "gubg/coding/d9/types.hpp"
#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "boost/msm/back/state_machine.hpp"
#include "boost/msm/front/state_machine_def.hpp"
#include "boost/msm/front/functor_row.hpp"
#include "boost/msm/front/completion_event.hpp"
#include "boost/msm/front/euml/common.hpp"
#include "boost/mpl/vector.hpp"
#include <ostream>
#include <iomanip>
using namespace gubg::coding;
using d9::ubyte;
using d9::Parser;
using d9::ReturnCode;
using namespace boost::msm;
using namespace boost::msm::front;
using namespace boost::msm::front::euml;
using namespace boost;
using namespace std;
#define L(m) cout<<m<<endl

namespace
{
    struct Byte;
    struct OK;
}
ostream &operator<<(ostream &os, const Byte &);
ostream &operator<<(ostream &os, const OK &);
namespace
{
    //Events
    struct Byte
    {
        Byte(ubyte v):value(v){}
        ubyte value;
        bool isD9() const
        {
            bool res = (d9::Byte::D9 == value);
            LOG_SM(isD9, "byte: " << *this << ", res: " << res);
            return res;
        }
    };
    struct OK {};
    struct ReadAttribute {};

    bool isEndRLE(const ubyte value)
    {
        return (value & 0x80) && !(value & 0x40);
    }
}
ostream &operator<<(ostream &os, const Byte &b)
{
    return os << "Byte(0x"<< hex << (int)b.value << ")";
}
ostream &operator<<(ostream &os, const OK &) { return os << "OK"; }
ostream &operator<<(ostream &os, const ReadAttribute &) { return os << "ReadAttribute"; }
template <typename T>
string to_s(const T &t)
{
    ostringstream oss;
    oss << typeid(t).name();
    return oss.str();
}
#define L_DEFINE_TO_S(type) string to_s(const type &v){ostringstream oss; oss << v; return oss.str();}
    L_DEFINE_TO_S(Byte);
    L_DEFINE_TO_S(OK);
    L_DEFINE_TO_S(ReadAttribute);
namespace
{
#define L_ENTRY_EXIT(name) \
        template <typename E, typename M> \
            void on_entry(const E &e, M &){L("Entering state " #name " via event " << to_s(e));} \
        template <typename E, typename M> \
            void on_exit(const E &e, M &){L("Leaving state " #name " via event " << to_s(e));}
    //States
    struct Idle: front::state<>
    {
        L_ENTRY_EXIT(Idle);
    };
    struct ReadingMeta: front::state<>
    {
        template <typename E, typename M>
            void on_entry(const E &e, M &m)
            {
                L("Entering state ReadingMeta via event " << to_s(e));
                if (!bytes.empty() && isEndRLE(bytes.back()))
                    m.process_event(OK());
            }
        template <typename E, typename M>
            void on_exit(const E &e, M &)
            {
                L("Leaving state ReadingMeta via event " << to_s(e));
            }
        string bytes;
        void clear(){bytes.clear();}
        size_t nrAttributes() const
        {
            size_t nr = 0;
            if (bytes.empty())
                return nr;
            auto b = bytes[0];
            for (size_t i = 1; i < 7; ++i)
                if (b & (0x01 << i))
                    ++nr;
            return nr;
        }
    };
    struct ReadingAttributes: front::state<>
    {
        template <typename E, typename M>
            void on_entry(const E &e, M &m)
            {
                LOG_SM(ReadingAttributes::on_entry, "nrAttributes: " << nrAttributes << ", attributes.size(): " << attributes.size());
                if (attributes.size() == nrAttributes)
                    m.process_event(OK());
                else
                    m.process_event(ReadAttribute());
            }
        template <typename E, typename M>
            void on_exit(const E &e, M &)
            {
                L("Leaving state ReadingAttributes via event " << to_s(e));
            }
        size_t nrAttributes;
        vector<string> attributes;
        void clear(){attributes.clear();}
    };
    struct ReadingAttribute: front::state<>
    {
        L_ENTRY_EXIT(ReadingAttribute);
        string bytes;
        void clear(){bytes.clear();}
    };
    struct ReadingData: front::state<>
    {
        L_ENTRY_EXIT(ReadingData);
        string bytes;
        void clear(){bytes.clear();}
    };
    struct ReadingChecksum: front::state<>
    {
        L_ENTRY_EXIT(ReadingChecksum);
    };
    struct MessageRead: front::state<>
    {
        L_ENTRY_EXIT(MessageRead);
    };

    //Finite statemachine
    struct Statemachine_: front::state_machine_def<Statemachine_>
    {
        void add_byte(const Byte &b)
        {
            LOG_SM(add_byte, "byte: " << b);
        }

        //Actions
        struct CheckStartOfMessage
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_SM(CheckStartOfMessage, "Event: " << e);
                    if (e.isD9())
                        m.process_event(OK());
                }
        };
        struct ClearTarget
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(ClearTarget);
                    t.clear();
                }
        };
        struct ClearTargetAndSetNrAttributes
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(ClearTargetAndSetNrAttributes);
                    t.clear();
                    t.nrAttributes = s.nrAttributes();
                }
        };
        struct AppendByte
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(Append);
                    t.bytes.push_back(e.value);
                }
        };
        struct AppendAttribute
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(Append);
                    t.attributes.push_back(s.bytes);
                }
        };
        typedef Idle initial_state;
        typedef Statemachine_ SM;
        struct transition_table: mpl::vector<
                                 Row<Idle, Byte, Idle, CheckStartOfMessage>,
                                 Row<Idle, OK, ReadingMeta, ClearTarget>,
                                 Row<ReadingMeta, Byte, ReadingMeta, AppendByte>,
                                 Row<ReadingMeta, OK, ReadingAttributes, ClearTargetAndSetNrAttributes>,
                                 Row<ReadingAttributes, ReadAttribute, ReadingAttribute, ClearTarget>,
                                 Row<ReadingAttributes, OK, ReadingData, ClearTarget>,
                                 Row<ReadingAttribute, Byte, ReadingAttribute, AppendByte>,
                                 Row<ReadingAttribute, OK, ReadingAttributes, AppendAttribute>
                                 >
        {
        };
    };
    typedef back::state_machine<Statemachine_> Statemachine;
}

Parser::Parser():
    state_(Idle){}

ReturnCode Parser::add(ubyte b)
{
    MSS_BEGIN(ReturnCode);
    MSS_END();
}

void Parser::clear()
{
    messages_.clear();
    state_ = Idle;
}

int main()
{
    MSS_BEGIN(int);
    LOG_S(test);
    Statemachine sm;
    sm.start();
    sm.process_event(Byte('a'));
    sm.process_event(Byte('\xd9'));
    sm.process_event(Byte('\x80'));
#if 0
    sm.process_event((ubyte)'b');
    sm.process_event((ubyte)'c');
#endif
    MSS_END();
}
