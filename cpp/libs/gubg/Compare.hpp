#ifndef HEADER_gubg_Compare_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Compare_hpp_ALREADY_INCLUDED

namespace gubg { 

	template <typename Ftor, typename Map>
		void keycompare_maps(Ftor &ftor, const Map &lhs, const Map &rhs)
		{
			auto lit = lhs.begin();
			auto rit = rhs.begin();
			const auto lend = lhs.end();
			const auto rend = rhs.end();
			while (true)
			{
				const bool lok = lit != lend;
				const bool rok = rit != rend;
				if (!rok and !lok)
					//Both iterators are at their end
					break;

				if (!rok)
				{
					//Only left items left
					ftor.keycompare_lhs(*lit++);
					continue;
				}
				if (!lok)
				{
					//Only right items left
					ftor.keycompare_rhs(*rit++);
					continue;
				}

				//Both iterators are valid: compare the keys
				const auto &lkey = lit->first;
				const auto &rkey = rit->first;
				if (lkey < rkey)
				{
					//Left key is smaller
					ftor.keycompare_lhs(*lit++);
					continue;
				}
				if (rkey < lkey)
				{
					//Right key is smaller
					ftor.keycompare_rhs(*rit++);
					continue;
				}

				//Keys are equal
				ftor.keycompare_both(*lit++, *rit++);
			}
		}

} 

#endif
