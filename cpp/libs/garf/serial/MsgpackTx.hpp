#ifndef HEADER_garf_serial_MsgpackTx_hpp_ALREADY_INCLUDED
#define HEADER_garf_serial_MsgpackTx_hpp_ALREADY_INCLUDED

#include "Arduino.h"
#include "gubg/FixedVector.hpp"
#include "gubg/cstdint.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/msgpack/Write.hpp"

namespace garf { namespace serial { 

    template <typename Receiver, size_t MaxDepth, size_t BufferSize>
        class MsgpackTx_crtp
        {
            public:
                void process()
                {
                    if (doInit_())
                        //First-time initialization
                        Serial.begin(9600);

                    tryToPrepareTxBuffer_();
                    if (txBuffer_.empty())
                        //Nothing to send
                        return;

                    //We send one byte per iteration
                    Serial.print((char)*it_++);

                    if (it_ == txBuffer_.end())
                        //This was the last byte, reset the txBuffer_
                        txBuffer_.clear();
                }

                template <typename T>
                    bool put(const T &t)
                    {
                        if (!serializer_.buffer().empty())
                            //Serialization buffer is not empty, we cannot add t now
                            return false;

                        //Serialize t into serializer_
                        {
                            auto c = serializer_.createComposer(1);
                            if (!c.ok())
                                return false;
                            const int rid = receiver_().roleId(t);
                            c.put(rid, t);
                        }

                        tryToPrepareTxBuffer_();

                        return true;
                    }

            private:
                Receiver &receiver_() {return static_cast<Receiver&>(*this);}

                void tryToPrepareTxBuffer_()
                {
                    if (!txBuffer_.empty())
                        //txBuffer_ not empty yet: we are still sending
                        return;
                    if (serializer_.buffer().empty())
                        //No serialized data is waiting
                        return;
                    //OK: we can swap serializer_ with txBuffer_
                    serializer_.swap(txBuffer_);
                    it_ = txBuffer_.begin();
                }

                gubg::OnlyOnce doInit_;

                typedef gubg::FixedVector<std::uint8_t, BufferSize> Buffer;

                //The buffer that is being sent now, if any
                Buffer txBuffer_;
                typename Buffer::iterator it_;

                typedef gubg::msgpack::Serializer<Buffer, MaxDepth> Serializer;
                Serializer serializer_;
        };

} } 

#endif
