#include "rinle/view/Font.hpp"

namespace rinle { namespace view { namespace font {

	size_t computeSize(size_t height, size_t nr)
	{
		return (height*0.65)/nr;
	}

} } }
