#ifndef HEADER_gubg_d9_Decoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_d9_Decoder_hpp_ALREADY_INCLUDED

#include "gubg/d9/Codes.hpp"
#include "gubg/d9/Types.hpp"

#define GUBG_MODULE_ "Decoder"
#include "gubg/log/begin.hpp"
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
                    Decoder_crtp(){reset();}

                    void reset()
                    {
                        //This makes sure no bytes are reported until a d9 was received
                        state_ = State::WaitForD9;
                    }

                    ReturnCode process(ubyte b)
                    {
                        MSS_BEGIN(ReturnCode);

                        //If we receive a D9, we start a new frame immediately
                        if (b == Byte::D9)
                        {
                            flips_.resize(0);
                            state_ = State::Flips;
                            receiver_().d9_start();
                            MSS_RETURN_OK();
                        }

                        switch (state_)
                        {
                            case State::WaitForD9:
                                //Maybe some garbage because we started tuning in the middle of a stream
                                break;
                            case State::Flips:
                                {
                                    //Store this flip byte
                                    {
                                        const auto s = flips_.size();
                                        flips_.push_back(b);
                                        if (s+1 != flips_.size())
                                        {
                                            //push_back() failed: container is too small
                                            state_ = State::WaitForD9;
                                            receiver_().d9_error(ReturnCode::TooManyFlips);
                                            MSS_L(TooManyFlips);
                                        }
                                    }

                                    if ((b & 0xc0) == 0x80)
                                    {
                                        //This is the last flip byte
                                        flip_ = flips_.begin();
                                        flipOffset_ = 0;
                                        state_ = State::Content;
                                    }
                                }
                                break;
                            case State::Content:
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
                                    receiver_().d9_ubyte(b);
                                }
                        }
                        MSS_END();
                    }

                    void d9_error(ReturnCode error){}

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
                void d9_start(){str_.clear();}
                void d9_error(ReturnCode error){}
                void d9_ubyte(ubyte b)
                {
                    const auto s = str_.size();
                    str_.push_back(b);
                }

                String &str(){return str_;}

            private:
                String str_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
