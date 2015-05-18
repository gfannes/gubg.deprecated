#include "fff/agents/Hasher.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/hash/MD5.hpp"
#include <iostream>
using namespace std;

#define GUBG_MODULE_ "Hasher"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 
	gubg::bbs::ReturnCode Hasher::process(gubg::bbs::Board &board)
	{
		MSS_BEGIN(gubg::bbs::ReturnCode);

		auto tvs = board.getFrom(ix_);
		ix_ += tvs.size();

		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.tag == "hash.tag")
			{
                tags_.insert(tv.value);
			}
		}

        if (tags_.empty())
            MSS_RETURN_OK();

		for (auto tv: board.getFrom(0))
		{
			if (false) {}
			else if (tags_.count(tv.tag) && hashPerTV_.count(tv) == 0)
			{
                using namespace gubg::hash::md5;
                std::string content; gubg::file::read(content, tv.value);
				Stream md5; md5 << content;
                const auto h = md5.hash_hex();
                board.addItem("hash.md5sum", h, tv);
                hashPerTV_[tv] = h;
			}
		}

		MSS_END();
	}
} } 
#include "gubg/log/end.hpp"
