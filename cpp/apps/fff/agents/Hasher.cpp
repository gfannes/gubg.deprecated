#include "fff/agents/Hasher.hpp"
#include "fff/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/hash/MD5.hpp"
#include <iostream>
using namespace std;

#define GUBG_MODULE_ "Hasher"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 
	ReturnCode Hasher::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		auto tvs = board.getFrom(ix_);
		ix_ += tvs.size();

		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.tag == Tag("hash.tag"))
			{
                tags_.insert(tv.value.as_string());
			}
		}

        if (tags_.empty())
            MSS_RETURN_OK();

		for (auto tv: board.getFrom(0))
		{
			if (false) {}
			else if (tags_.count(tv.tag.to_str()) && hashPerTV_.count(tv) == 0)
			{
                using namespace gubg::hash::md5;
                std::string content; gubg::file::read(content, tv.value.as_file());
				Stream md5; md5 << content;
                const auto h = md5.hash_hex();
                board.addItem(Tag("hash.md5sum"), Value(h), tv);
                hashPerTV_[tv] = h;
			}
		}

		MSS_END();
	}
} } 
#include "gubg/log/end.hpp"
