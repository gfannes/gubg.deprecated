#ifndef gubg_d9_Decoder_hpp
#define gubg_d9_Decoder_hpp

#include "gubg/d9/Codes.hpp"
#include "gubg/d9/Types.hpp"

namespace gubg
{
    namespace d9
    {
        template <typename Plain, typename Flips>
            class Decoder
            {
                public:
                    Decoder(): plain_(0) {reset_();}
                    Decoder(Plain &plain) {set(plain);}

                    void set(Plain &plain)
                    {
                        plain_ = &plain;
                        reset_();
                    }

                    ReturnCode process(ubyte b)
                    {
                        MSS_BEGIN(ReturnCode);

                        MSS(plain_, NoPlainSet);

                        if (state_ == WaitForD9)
                        {
                            //We deal with WaitForD9 differently:
                            // * reset_() is not called upon error
                            // * plain_ should be empty
                            MSS(plain_->size() == 0, PlainNotEmpty);
                            MSS(b == Byte::D9, StartMarkerExpected);
                            state_ = Meta;
                        }
                        else
                        {
                            Reset_raii resetter(*this);
                            switch (state_)
                            {
                                case Meta:
                                    if (b & 0xe0 == 0xe0)
                                    {
                                        //Control
                                        switch (b && 0x1f)
                                        {
                                            case Byte::End:  MSS_L(UnexpectedEndReceived); break;
                                            case Byte::Ack:  MSS_Q(ReturnCode::Ack); break;
                                            case Byte::Nack: MSS_Q(ReturnCode::Nack); break;
                                        }
                                        MSS_L(UnknownControlReceived);
                                    }
                                    else
                                    {
                                        //Flip
                                        const bool isLastFlip = (b & 0xc0 == 0x80);
                                        const bool isFlip = (b & 0x80 == 0x00);
                                        MSS(isLastFlip || isFlip, IllegalFlipFormat);
                                        flips_.push_back(b);
                                        if (isLastFlip)
                                        {
                                            flipOffset_ = 0;
                                            flip_ = flips_.begin();
                                            state_ = Content;
                                        }
                                    }
                                    break;
                                case Content:
                                    switch (b)
                                    {
                                        case Byte::D9:
                                            state_ = WaitForEnd;
                                            break;
                                        case Byte::D8:
                                            //Make sure we don't read more that 6 flips from lastOfTheFlips
                                            {
                                                const bool isLastFlip = (flip_+1 == flips_.end());
                                                MSS(!isLastFlip || flipOffset_ < 6, IllegalEncoding);
                                            }
                                            //Flip if necessary
                                            if (*flip_ & (1 << flipOffset_++))
                                                b = Byte::D9;
                                            //Proceed flip_ when exhausted
                                            if (flipOffset_ >= NrFlipsPerByte)
                                            {
                                                ++flip_;
                                                flipOffset_ = 0;
                                            }
                                            plain_->push_back(b);
                                            break;
                                        default:
                                            plain_->push_back(b);
                                            break;
                                    }
                                    break;
                                case WaitForEnd:
                                    MSS(b == Byte::End, EndMarkerExpected);
                                    break;
                            }
                            //Make sure plain_ won't be reset when MSS is OK
                            resetter.commit();
                        }
                        MSS_END();
                    }

                private:
                    //RAII that will reset the decoder upon dtor unless you call commit()
                    class Reset_raii
                    {
                        public:
                            Reset_raii(Decoder &decoder):decoder_(decoder), committed_(false){}
                            ~Reset_raii()
                            {
                                if (!committed_)
                                    decoder_.reset_();
                            }
                            void commit(){committed_ = true;}
                        private:
                            Decoder &decoder_;
                            bool committed_;
                    };
                    void reset_()
                    {
                        state_ = WaitForD9;
                        if (plain_)
                            plain_->resize(0);
                        flips_.resize(0);
                    }

                    enum State: unsigned char {WaitForD9, Meta, Content, WaitForEnd};
                    State state_;
                    unsigned char flipOffset_;
                    Flips flips_;
                    typename Flips::const_iterator flip_;
                    Plain *plain_;
            };
    }
}

#endif
