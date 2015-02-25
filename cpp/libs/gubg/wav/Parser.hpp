#ifndef HEADER_gubg_wav_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wav_Parser_hpp_ALREADY_INCLUDED

#include "gubg/wav/Codes.hpp"
#include "gubg/wav/Format.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Strange.hpp"
#include <string>
#include <vector>

#define GUBG_MODULE_ "wav::Parser"
#include "gubg/log/begin.hpp"
namespace gubg { namespace wav { 

    template <typename Receiver>
        class Parser_crtp
        {
            public:
                ReturnCode operator()(const file::File &file)
                {
                    MSS_BEGIN(ReturnCode);
                    std::string content;
                    MSS(file::read(content, file));
                    MSS(operator()(content));
                    MSS_END();
                }
                ReturnCode operator()(const std::string &content)
                {
                    MSS_BEGIN(ReturnCode);
                    L(content.size());

                    Strange strange(content);

                    MSS(strange.popStringIf("RIFF"));
                    uint32_t file_size;
                    MSS(strange.popLSB(file_size));
                    L(STREAM(file_size));
                    MSS(strange.size() == file_size);
                    MSS(strange.popStringIf("WAVE"));

                    Format fmt;
                    while (!strange.empty())
                    {
                        std::string id;
                        Strange subchunk;
                        MSS(popSubchunk_(id, subchunk, strange));
                        if (false) {}
                        else if (id == "fmt ")
                        {
                            MSS(subchunk.popLSB(fmt.format));
                            MSS(subchunk.popLSB(fmt.channel_count));
                            MSS(subchunk.popLSB(fmt.sample_rate));
                            MSS(subchunk.popLSB(fmt.byte_rate));
                            MSS(subchunk.popLSB(fmt.block_size));
                            MSS(subchunk.popLSB(fmt.bits_per_sample));
                            receiver_().wav_format(fmt);
                        }
                        else if (id == "data")
                        {
                            Strange block;
                            typedef std::vector<std::int16_t> PCM16;
                            PCM16 pcm16(fmt.channel_count);
                            while (!subchunk.empty())
                            {
                                MSS(subchunk.popCount(block, fmt.block_size));
                                switch (fmt.format)
                                {
                                    case 1://PCM
                                        switch (fmt.bits_per_sample)
                                        {
                                            case 16:
                                                for (int ch = 0; ch < fmt.channel_count; ++ch)
                                                    MSS(block.popLSB(pcm16[ch]));
                                                receiver_().wav_sample(pcm16);
                                                break;
                                            default:
                                                MSS(ReturnCode::UnsupportBitsPerSample);
                                                break;
                                        }
                                        break;
                                    default:
                                        MSS(ReturnCode::UnsupportFormat);
                                        break;
                                }
                            }
                        }
                    }
                    receiver_().wav_done();

                    MSS_END();
                }

            private:
                Receiver &receiver_() {return static_cast<Receiver&>(*this);}

                ReturnCode popSubchunk_(std::string &id, Strange &subchunk, Strange &strange)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS(strange.popString(id, 4));
                    uint32_t size;
                    MSS(strange.popLSB(size));
                    L(STREAM(id, size));
                    MSS(strange.popCount(subchunk, size));
                    MSS_END();
                }
        };

} } 
#include "gubg/log/end.hpp"

#endif
