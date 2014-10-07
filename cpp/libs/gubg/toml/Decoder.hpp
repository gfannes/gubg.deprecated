#ifndef HEADER_gubg_toml_Decoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_toml_Decoder_hpp_ALREADY_INCLUDED

#include "gubg/toml/Codes.hpp"
#include "gubg/Range.hpp"
#include <string>
#include <stack>
#include <cassert>

#define GUBG_MODULE_ "Decoder"
#include "gubg/log/begin.hpp"
namespace gubg { namespace toml { 

	template <typename Receiver>
		class Decoder_crtp
		{
			public:
				ReturnCode decode(const std::string &msg)
				{
					MSS_BEGIN(ReturnCode);
					range_ = make_range(msg);
					path_ = Path();
					assert(path_.empty());
					for (; !range_.empty(); range_.popFront())
					{
						L(range_.front());
						if (isWhitespace_())
							continue;
					}
					MSS_END();
				}

			private:
				Receiver &receiver_(){return static_cast<Receiver&>(*this);}
				bool isWhitespace_() const
				{
					assert(!range_.empty());
					switch (range_.front())
					{
						case '\t':
						case ' ':
							return true;
					}
					return false;
				}

				typedef gubg::Range<std::string::const_iterator> Range;
				Range range_;
				typedef std::stack<Range> Path;
				Path path_;
		};

} } 
#include "gubg/log/end.hpp"

#endif
