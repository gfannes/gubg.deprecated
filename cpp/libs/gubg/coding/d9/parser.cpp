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
#include <vector>
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
            LOG_S(isD9, STREAM(*this, res));
            return res;
        }
    };
    struct OK {};
    struct Fail {};
    struct Skipped {};
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
ostream &operator<<(ostream &os, const Fail &) { return os << "Fail"; }
ostream &operator<<(ostream &os, const Skipped &) { return os << "Skipped"; }
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
    L_DEFINE_TO_S(Fail);
    L_DEFINE_TO_S(Skipped);
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
                if (!m.meta_.empty() && isEndRLE(m.meta_.back()))
                    m.process_event(OK());
            }
        template <typename E, typename M>
            void on_exit(const E &e, M &)
            {
                L("Leaving state ReadingMeta via event " << to_s(e));
            }
    };
    struct ReadingAttributes: front::state<>
    {
        template <typename E, typename M>
            void on_entry(const E &e, M &m)
            {
                LOG_S(ReadingAttributes::on_entry);
                if (m.attribute_ == m.attributes_.end())
                    m.process_event(OK());
                else
                    m.process_event(ReadAttribute());
            }
        template <typename E, typename M>
            void on_exit(const E &e, M &)
            {
                L("Leaving state ReadingAttributes via event " << to_s(e));
            }
    };
    struct ReadingAttribute: front::state<>
    {
        L_ENTRY_EXIT(ReadingAttribute);
        string bytes;
        void clear(){bytes.clear();}
    };
    struct ReadingData: front::state<>
    {
        template <typename E, typename M>
            void on_entry(const E &e, M &m)
            {
                LOG_S(ReadingData::on_entry, STREAM((bool)m.data_));
                if (!m.data_)
                    m.process_event(Skipped());
            }
        template <typename E, typename M>
            void on_exit(const E &e, M &)
            {
                L("Leaving state ReadingData via event " << to_s(e));
            }
    };
    struct ReadingChecksum: front::state<>
    {
        template <typename E, typename M>
            void on_entry(const E &e, M &m)
            {
                LOG_S(ReadingChecksum::on_entry, STREAM(m.expectChecksum_, (int)m.checksum_));
                if (!m.expectChecksum_)
                    m.process_event(Skipped());
            }
        template <typename E, typename M>
            void on_exit(const E &e, M &)
            {
                L("Leaving state ReadingChecksum via event " << to_s(e));
            }
    };
    struct MessageRead: front::state<>
    {
        L_ENTRY_EXIT(MessageRead);
    };

    //Finite statemachine
    struct Statemachine_: front::state_machine_def<Statemachine_>
    {
        void add_byte(const Byte &byte)
        {
            LOG_S(add_byte, STREAM(byte));
        }

        //Actions
        struct CheckStartOfMessage
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(CheckStartOfMessage);
                    if (e.isD9())
                    {
                        LOG_M("I found the start of a message and will switch to ReadingMeta");
                        m.initializeChecksum_(e);
                        m.process_event(OK());
                    }
                    else
                        LOG_M("This is not the start of a message, I will remain Idle");
                }
        };
        struct ClearMachine
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(ClearMachine);
                    m.clear();
                }
        };
        struct AppendToMeta
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(AppendToMeta, STREAM(e));
                    m.meta_.push_back(e.value);
                    m.updateChecksum_(e);
                    LOG_M(STREAM(m.meta_.size()));
                }
        };
        struct AllocateAttributes
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(AllocateAttributes);
                    m.attributes_.resize(m.nrAttributes());
                    m.attribute_ = m.attributes_.begin();
                    LOG_M(STREAM(m.attributes_.size()));
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
        struct AppendByte
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(Append, STREAM(e));
                    t.bytes.push_back(e.value);
                    m.updateChecksum_(e);
                }
        };
        struct AppendAttribute
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(AppendAttribute, STREAM(s.bytes.size()));
                    m.attribute_->swap(s.bytes);
                    ++m.attribute_;
                }
        };
        struct AppendByteToData
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(AppendByteToData, STREAM(e));
                    MSS_ENSURE((bool)m.data_, "Expected data to be valid");
                    m.data_->push_back(e.value);
                    m.updateChecksum_(e);
                }
        };
        struct CheckChecksum
        {
            template <typename E, typename M, typename S, typename T>
                void operator()(const E &e, M &m, S &s, T &t)
                {
                    LOG_S(CheckChecksum, STREAM(e, m.checksum_));
                    if ((e.value & 0x7f) == (m.checksum_ & 0x7f))
                        m.process_event(OK());
                    else
                        m.process_event(Fail());
                }
        };
        typedef Idle initial_state;
        typedef Statemachine_ SM;
        struct transition_table: mpl::vector<
                                 Row<Idle, Byte, Idle, CheckStartOfMessage>,
                                 Row<Idle, OK, ReadingMeta, ClearMachine>,
                                 Row<ReadingMeta, Byte, ReadingMeta, AppendToMeta>,
                                 Row<ReadingMeta, OK, ReadingAttributes, AllocateAttributes>,
                                 Row<ReadingAttributes, ReadAttribute, ReadingAttribute, ClearTarget>,
                                 Row<ReadingAttribute, Byte, ReadingAttribute, AppendByte>,
                                 Row<ReadingAttribute, OK, ReadingAttributes, AppendAttribute>,
                                 Row<ReadingAttributes, OK, ReadingData>,
                                 Row<ReadingData, Byte, ReadingData, AppendByteToData>,
                                 Row<ReadingData, OK, ReadingChecksum>,
                                 Row<ReadingData, Skipped, ReadingChecksum>,
                                 Row<ReadingChecksum, Byte, ReadingChecksum, CheckChecksum>,
                                 Row<ReadingChecksum, Skipped, MessageRead>,
                                 Row<ReadingChecksum, OK, MessageRead>,
                                 Row<ReadingChecksum, Fail, Idle, ClearMachine>
                                 >
        {
        };

        void clear()
        {
            meta_.clear();
            attributes_.clear();
            attribute_ = attributes_.end();
            data_.reset();
            expectChecksum_ = false;
        }
        string meta_;
        typedef vector<string> Attributes;
        Attributes attributes_;
        Attributes::iterator attribute_;
        unique_ptr<string> data_;
        bool expectChecksum_;
        ubyte checksum_;
        void initializeChecksum_(Byte b){LOG_S(initializeChecksum_, b); checksum_ = b.value;}
        void updateChecksum_(Byte b){LOG_S(updateChecksum_, b); checksum_ ^= b.value;}

        size_t nrAttributes() const
        {
            size_t nr = 0;
            if (meta_.empty())
                return nr;
            auto b = meta_[0];
            for (size_t i = 1; i < 7; ++i)
                if (b & (0x01 << i))
                    ++nr;
            return nr;
        }
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
