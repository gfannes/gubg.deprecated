#ifndef HEADER_gubg_base64_Decode_hpp_ALREADY_INCLUDED
#define HEADER_gubg_base64_Decode_hpp_ALREADY_INCLUDED

#include "gubg/base64/Codes.hpp"
#include <string>
#include <cassert>

#define GUBG_MODULE "Decode"
#include "gubg/log/begin.hpp"
namespace gubg { namespace base64 { 

	namespace details { 
		ReturnCode translate(uint8_t &tr, char ch)
		{
			if (false) {}
			else if ('A' <= ch && ch <= 'Z')
				tr = ch-'A';
			else if ('a' <= ch && ch <= 'z')
				tr = 26+ch-'a';
			else if ('0' <= ch && ch <= '9')
				tr = 52+ch-'0';
			else if (ch == '+')
				tr = 62;
			else if (ch == '/')
				tr = 63;
			else
				return ReturnCode::InvalidEncodedChar;
			return ReturnCode::OK;
		}
	} 

	template <typename Receiver>
		class Decoder_crtp
		{
			public:
				ReturnCode process(char ch)
				{
					MSS_BEGIN(ReturnCode, ch);
					if (ch == '=')
						MSS_RETURN_OK();
					uint8_t tr;
					MSS(details::translate(tr, ch));
					L((int)tr);
					switch (state_)
					{
						case 0:
							dec_ = (tr << 2);
							++state_;
							break;
						case 1:
							dec_ |= (tr >> 4);
							receiver_().base64_decoded(dec_);
							dec_ = (tr << 4);
							++state_;
							break;
						case 2:
							dec_ |= (tr >> 2);
							receiver_().base64_decoded(dec_);
							dec_ = (tr << 6);
							++state_;
							break;
						case 3:
							dec_ |= tr;
							receiver_().base64_decoded(dec_);
							state_ = 0;
							break;
					}
					MSS_END();
				}

			private:
				Receiver &receiver_(){return static_cast<Receiver&>(*this);}
				int state_ = 0;
				uint8_t dec_;
		};

	namespace details { 

		class Decoder: public Decoder_crtp<Decoder>
		{
			public:
				std::string decoded;

				ReturnCode decode(const std::string &enc)
				{
					MSS_BEGIN(ReturnCode);
					MSS(allocate_decoded_(enc));
					dst_ = decoded.begin();
					for (auto ch: enc)
						MSS(process(ch));
					assert(dst_ == decoded.end());
					MSS_END();
				}
				void base64_decoded(uint8_t dec)
				{
					*dst_++ = (char)dec;
				}

			private:
				ReturnCode allocate_decoded_(const std::string &enc)
				{
					MSS_BEGIN(ReturnCode);
					if (!enc.empty())
					{
						size_t s = enc.size();
						MSS(s%4 == 0, IncorrectSize);
						s = (s/4)*3;
						auto it = enc.end();
						if (*--it == '=')
							--s;
						if (*--it == '=')
							--s;
						L(STREAM(s));
						decoded.resize(s);
					}
					MSS_END();
				}
				std::string::iterator dst_;
		};
	} 

	ReturnCode decode(std::string &dec, const std::string &enc)
	{
		MSS_BEGIN(ReturnCode);
		details::Decoder decoder;
		MSS(decoder.decode(enc));
		decoder.decoded.swap(dec);
		MSS_END();
	}

} } 
#include "gubg/log/end.hpp"

#endif
