#include "gubg/dynlib/Library.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
#include <map>
#include <cassert>
using namespace gubg::file;
using namespace std;

namespace  { 
    typedef void* Symbol;
} 

#ifdef GUBG_POSIX
#include "dlfcn.h"
namespace  { 
    typedef void* Handle;
    bool load_(Handle &h, const string &fn) { h = ::dlopen(fn.c_str(), RTLD_NOW); return h != 0; }
    bool unload_(Handle h) { return ::dlclose(h) == 0; }
    Symbol get_address_(Handle h, const string &name) {return ::dlsym(h, name.c_str());}
} 
#endif
#ifdef GUBG_WIN32
#include "Windows.h"
namespace  { 
    typedef HMODULE Handle;
    bool load_(Handle &h, const string &fn) { h = ::LoadLibrary(fn.c_str()); return h != 0; }
    bool unload_(Handle h) { return ::FreeLibrary(h); }
    Symbol get_address_(Handle h, const string &name) {return ::GetProcAddress(h, name.c_str());}
} 
#endif

#define GUBG_MODULE "dynlib"
#include "gubg/log/begin.hpp"
namespace gubg { namespace dynlib { 

    typedef map<string, Symbol> SymbolPerName;

	struct Library::Pimpl
	{
		Handle h;
		File fn;
        mutable SymbolPerName symbolPerName;

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

    void *Library::get(const std::string &name) const
    {
        if (!pimpl_)
            return 0;
        const auto &pimpl = *pimpl_;

        {
            auto p = pimpl.symbolPerName.find(name);
            if (p != pimpl.symbolPerName.end())
                return p->second;
        }

        const auto sym = get_address_(pimpl.h, name);
        if (!!sym)
            pimpl.symbolPerName[name] = sym;

        return sym;
    }

} } 
#include "gubg/log/end.hpp"
