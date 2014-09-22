#include "gubg/dynlib/Library.hpp"
#include "gubg/file/Filesystem.hpp"
#include "Windows.h"
#include <cassert>
using namespace gubg::file;

#define GUBG_MODULE "dynlib"
#include "gubg/log/begin.hpp"
namespace gubg { namespace dynlib { 

	struct Library::Pimpl
	{
		HMODULE h;
		File fn;
		Pimpl(HMODULE h, const File &fn): h(h), fn(fn)
		{
			assert(h != 0);
		}
		~Pimpl()
		{
			const bool ok = ::FreeLibrary(h);
			assert(ok);
		}
	};

	Library::Library(const File &fn)
	{
		ctor_(fn);
	}
	ReturnCode Library::ctor_(File fn)
	{
		MSS_BEGIN(ReturnCode);
		MSS(resolve(fn));
		MSS(isRegular(fn));
		HMODULE h = ::LoadLibrary(fn.name().c_str());
		MSS(h != 0);
		pimpl_.reset(new Pimpl(h, fn));
		MSS_END();
	}

} } 
#include "gubg/log/end.hpp"
