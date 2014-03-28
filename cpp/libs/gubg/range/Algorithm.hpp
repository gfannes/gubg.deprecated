#ifndef HEADER_gubg_range_Algorithm_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_Algorithm_hpp_ALREADY_INCLUDED

namespace gubg { namespace range { 

	//On failure, the content of dst might be changed, but its layout won't
	template <typename DestinationRange, typename SourceRange>
		bool append(DestinationRange &dst, SourceRange src)
		{
			DestinationRange l_dst = dst;
			for (; !src.empty(); src.popFront())
			{
				if (l_dst.empty())
					//No more space left in dest
					return false;
				//This changes the content of dst too
				l_dst.front() = src.front();
				l_dst.popFront();
			}
			//All ok: adjust dst
			dst = l_dst;
			return true;
		}

} } 

#endif
