#include "gubg/dynlib/Library.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
#include <cassert>
using namespace gubg::file;

#ifdef GUBG_POSIX
#include "dlfcn.h"
namespace  { 
    typedef void* Handle;
    bool load_(Handle &h, const std::string &fn) { h = ::dlopen(fn.c_str(), RTLD_NOW); return h != 0; }
    bool unload_(Handle &h) { return ::dlclose(h) == 0; }
} 
#endif
#ifdef GUBG_WIN32
#include "Windows.h"
namespace  { 
    typedef HMODULE Handle;
    bool load_(Handle &h, const std::string &fn) { h = ::LoadLibrary(fn.c_str()); return h != 0; }
    bool unload_(Handle &h) { return ::FreeLibrary(h); }
} 
#endif

#define GUBG_MODULE "dynlib"
#include "gubg/log/begin.hpp"
namespace gubg { namespace dynlib { 

	struct Library::Pimpl
	{
		Handle h;
		File fn;
		Pimpl(Handle h, const File &fn): h(h), fn(fn) { }
		~Pimpl() { unload_(h); }
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
        Handle h;
        MSS(load_(h, fn.name()));
		pimpl_.reset(new Pimpl(h, fn));
		MSS_END();
	}

} } 
#include "gubg/log/end.hpp"
