#ifndef HEADER_gubg_pattern_SignalSlot_hpp_ALREADY_INCLUDED
#define HEADER_gubg_pattern_SignalSlot_hpp_ALREADY_INCLUDED

#include <mutex>
#include <vector>

#define GUBG_MODULE "SignalSlot"
#include "gubg/log/begin.hpp"
namespace gubg { namespace pattern {

    template <typename Msg>
        class Signal;

    namespace priv
    {
        template <typename Msg>
            class Slot_itf
            {
                public:
                    //Disconnects itself from all connected signals
                    virtual ~Slot_itf();

                    virtual void process(Msg) = 0;

                private:
                    friend class Signal<Msg>;
                    void connect_(Signal<Msg> *);
                    void disconnect_(Signal<Msg> *);

                    typedef Signal<Msg> *SignalPtr;
                    typedef std::vector<SignalPtr> Signals;
                    Signals signals_;
                    typedef std::mutex Mutex;
                    typedef std::lock_guard<Mutex> LockGuard;
                    Mutex mutex_;
            };
    }

    template <typename Msg>
        class Signal
        {
            public:
                ~Signal();

                void emit(Msg);

                template <typename Slot>
                    void connect(Slot &);

            private:
                friend class priv::Slot_itf<Msg>;
                void disconnect_(priv::Slot_itf<Msg> *);

                typedef typename priv::Slot_itf<Msg> *SlotPtr;
                typedef std::vector<SlotPtr> Slots;
                Slots slots_;
                //We need a recursive mutex because processing a msg might add new slots.
                typedef std::recursive_mutex Mutex;
                typedef std::lock_guard<Mutex> LockGuard;
                Mutex mutex_;
        };

    template <typename Receiver, typename Msg>
        class Slot: public priv::Slot_itf<Msg>
        {
            public:
                Slot(Receiver &receiver): receiver_(receiver) {}

                //Make sure no slots are dtorred during processing of this msg
                virtual void process(Msg);

            private:
                Receiver &receiver_;
        };

    //Implementation of Signal
    template <typename Msg>
        Signal<Msg>::~Signal()
        {
			S();
            LockGuard lg(mutex_);
            for (auto slot: slots_)
                slot->disconnect_(this);
        }
    template <typename Msg>
        void Signal<Msg>::emit(Msg msg)
        {
			S();
            LockGuard lg(mutex_);
            //We take a copy of the slots, this allows new slots to be added as a result of processing. This is also the reason
            //why a Signal uses a recursive mutex, new slots might be added recursively.
            //Make sure no slots are removed though during processing.
            Slots slots = slots_;
            for (auto slot: slots)
			{
				L("Calling process on " << slot);
                slot->process(msg);
			}
        }
    template <typename Msg>
        template <typename Slot>
        void Signal<Msg>::connect(Slot &slot)
        {
            LockGuard lg(mutex_);
            slots_.push_back(&slot);
			slot.connect_(this);
        }
    template <typename Msg>
        void Signal<Msg>::disconnect_(priv::Slot_itf<Msg> *slot)
        {
            //This method is _only_ called from ~Slot_itf, make sure we cannot deadlock, ensure a fixed order of locking
            //is in place, which is probably not the case right now
            LockGuard lg(mutex_);
            for (typename Slots::iterator it = slots_.begin(); it != slots_.end();)
            {
                if (*it == slot)
                    it = slots_.erase(it);
                else
                    ++it;
            }
        }

    //Implementation of Slot
    namespace priv
    {
        template <typename Msg>
            Slot_itf<Msg>::~Slot_itf()
            {
				S();L("I am dying: " << this);
                //TODO::I have the feeling this could lead to deadlocks, make sure there is always a fixed
                //order between getting our mutex and the one from signal during the disconnect_() call
                LockGuard lg(mutex_);
                for (auto signal: signals_)
				{
					L("Disconnecting myself (" << this << ") from signal " << signal);
                    signal->disconnect_(this);
				}
            }
        template <typename Msg>
            void Slot_itf<Msg>::connect_(Signal<Msg> *signal)
            {
                LockGuard lg(mutex_);
				signals_.push_back(signal);
            }
        template <typename Msg>
            void Slot_itf<Msg>::disconnect_(Signal<Msg> *signal)
            {
                LockGuard lg(mutex_);
                for (typename Signals::iterator it = signals_.begin(); it != signals_.end();)
                {
                    if (*it == signal)
                        it = signals_.erase(it);
                    else
                        ++it;
                }
            }
    }


    //Implementation of Slot
    template <typename Receiver, typename Msg>
        void Slot<Receiver, Msg>::process(Msg msg)
        {
            //We just forward the msg to receiver_
            receiver_.process(msg);
        }

} }
#include "gubg/log/end.hpp"

#endif
