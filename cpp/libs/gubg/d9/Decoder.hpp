#ifndef HEADER_gubg_d9_Decoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_d9_Decoder_hpp_ALREADY_INCLUDED

#include "gubg/d9/Codes.hpp"
#include "gubg/d9/Types.hpp"

namespace gubg
{
    namespace d9
    {
        //CRTP decoder
        //Receiver should provide:
        // * ReturnCode d9_start() => OK
        // * ReturnCode d9_ubyte(ubyte b) => ContentComplete or OK
        // * void d9_error(ReturnCode error)
        //FlipsT is a normal template parameter which specifies how the flip bytes are cached
        template <typename Receiver, typename FlipsT>
            class Decoder_crtp
            {
                public:
                    Decoder_crtp():
                        state_(State::WaitForD9){}

                    ReturnCode process(ubyte b)
                    {
                        MSS_BEGIN(ReturnCode);

                        if (state_ == State::WaitForD9)
                        {
                            //Errors that happen here are not reported to Receiver
                            MSS(b == Byte::D9, StartMarkerExpected);
                            MSS(receiver_().d9_start());
                            flips_.resize(0);
                            state_ = State::Flips;
                        }
                        else
                        {
                            if (b == Byte::D9)
                            {
                                state_ = State::WaitForD9;
                                receiver_().d9_error(ReturnCode::UnexpectedD9Received);
                                MSS_L(UnexpectedD9Received);
                            }
                            if (state_ == State::Flips)
                            {
                                if ((b & 0x80) == 0x00)
                                {
                                    //This is a flip byte
                                    const auto s = flips_.size();
                                    flips_.push_back(b);
                                    if (s+1 != flips_.size())
                                    {
                                        state_ = State::WaitForD9;
                                        receiver_().d9_error(ReturnCode::TooManyFlips);
                                        MSS_L(TooManyFlips);
                                    }
                                }
                                else
                                {
                                    //This is the first content byte, we deal with it hereunder
                                    //after we initialize necessary members
                                    flip_ = flips_.begin();
                                    flipOffset_ = 0;
                                    state_ = State::Content;
                                }
                            }
                            if (state_ == State::Content)
                            {
                                if (b == Byte::D8)
                                {
                                    if (flip_ == flips_.end())
                                    {
                                        state_ = State::WaitForD9;
                                        receiver_().d9_error(ReturnCode::TooManyDx);
                                        MSS_L(TooManyDx);
                                    }
                                    //Flip if necessary
                                    if (*flip_ & (1 << flipOffset_++))
                                        b = Byte::D9;
                                    //Proceed flip when exhausted
                                    if (flipOffset_ >= NrFlipsPerByte)
                                    {
                                        ++flip_;
                                        flipOffset_ = 0;
                                    }
                                }
                                switch (receiver_().d9_ubyte(b))
                                {
                                    case ReturnCode::OK:
                                        break;
                                    case ReturnCode::ContentComplete:
                                        state_ = State::WaitForD9;
                                        break;
                                    default:
                                        state_ = State::WaitForD9;
                                        receiver_().d9_error(ReturnCode::UnexpectedReply);
                                        MSS_L(UnexpectedReply);
                                        break;
                                }
                            }
                        }
                        MSS_END();
                    }

                    void reset()
                    {
                        state_ = State::WaitForD9;
                    }

                private:
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                    enum class State: unsigned char {WaitForD9, Flips, Content};
                    State state_;
                    unsigned char flipOffset_;
                    FlipsT flips_;
                    typename FlipsT::const_iterator flip_;
            };

        //A decoder example that will decode into its own instance of String by using push_back
        //No msgpack parsing is done, so this decoder doesn't know when the received content is complete
        template <typename String>
            class StringDecoder: public Decoder_crtp<StringDecoder<String>, String>
        {
            public:
                ReturnCode d9_start(){str_.clear(); return ReturnCode::OK;}
                void d9_error(ReturnCode error){}
                ReturnCode d9_ubyte(ubyte b)
                {
                    const auto s = str_.size();
                    str_.push_back(b);
                    return (s+1 == str_.size() ? ReturnCode::OK : ReturnCode::PushBackFailed);
                }

                String &str(){return str_;}

            private:
                String str_;
        };
    }
}

#endif
