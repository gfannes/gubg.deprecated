" Cpp mappings
map -y <Bslash>aa" <<  << "<Esc><Bslash>a4hi

" include ""
map -r <Bslash>aO#include "<Esc>:call ReadString("Header")<CR>A"<Esc>j<Bslash>a
" include <>
map -R <Bslash>aO#include <<Esc>:call ReadString("Header")<CR>A><Esc>j<Bslash>a
" guard
map -g <Bslash>ai#ifndef <Esc>:call ReadString("Path to header")<CR>$by$o#define <Esc>p-o-oo#endif<Esc>kk<Bslash>a
" block
map -b -o<Bslash>aa{<Esc>-oa}<Esc><Bslash>ak$
" class
map -c <Bslash>aoclass <Esc>:call ReadString("Class name")<CR>-oa{<Esc>-oapublic:<Esc>-oaprivate:<Esc>-oa};<Esc><Bslash>akk$
" struct
map -C <Bslash>aostruct <Esc>:call ReadString("Struct name")<CR>-oa{<Esc>-oa};<Esc><Bslash>ak$
" namespace
map -s <Bslash>aAnamespace <Esc>:call ReadString("Namespace name")<CR>A { <Esc>-oa}<Esc>J<Bslash>ak$
" switch
map -S <Bslash>aoswitch (<Esc>:call ReadString("Switcher")<CR>A)<Esc>o{<Esc>ocase : break;<Esc>o}<Esc><Bslash>akhhhhhhh
" for
map -f <Bslash>aofor (<Esc>:call ReadString("Type")<CR>A::iterator it = <Esc>:call ReadString("Container")<CR>A.begin(); it != .end(); ++it)<Esc>o{<Esc>o}<Esc><Bslash>akhhhhhhh
" template
map -t <Bslash>aotemplate <typename <Esc>:call ReadString("Type")<CR>A><Esc><Bslash>a$

let hn = hostname()
let athome = 0
let indep = 1
if hn == "rii"
	let athome = 1
endif
if hn == "riii"
	let athome = 1
endif
if athome
	map -p <Bslash>aoLOG_M();<Esc><Bslash>ahhi
	map -P <Bslash>aoLOG_M_(<Esc>:call ReadString("Log level")<CR>A, "");<Esc><Bslash>ahhhi
    map -l <Bslash>ao#define GUBG_MODULE "<Esc>:call ReadString("Module name")<CR>A"<Esc>o#include "gubg/log/begin.hpp"<Esc>o#include "gubg/log/end.hpp"<Esc><Bslash>ak$
	" MSS
	map -n <Bslash>ajOMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc><Bslash>ak$
	map -N <Bslash>ajOMSS_BEGIN(<Esc>:call ReadString("ReturnCode")<CR>A);<Esc>oMSS_END();<Esc><Bslash>ak$
	map -m <Bslash>ajOMSS();<Esc><Bslash>ahi
	map -v <Bslash>aoReturnCode<Esc>o{<Esc>oMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc>o}<Esc>kkkk<Bslash>aA 
else
    map -l <Bslash>ao#define ID_NAME "<Esc>:call ReadString("Indep debug name")<CR>A"<Esc>o#include "indep/debug/begin.h"<Esc>o#include "indep/debug/end.h"<Esc><Bslash>ak$
	if indep
		" INDEP_MSS
		map -n <Bslash>ajOINDEP_MSS_BEGIN(ReturnCode);<Esc>oINDEP_MSS_END();<Esc><Bslash>ak$
		map -N <Bslash>ajOINDEP_MSS_BEGIN(<Esc>:call ReadString("ReturnCode")<CR>A);<Esc>oINDEP_MSS_END();<Esc><Bslash>ak$
		map -m <Bslash>ajOINDEP_MSS();<Esc><Bslash>ahi
		map -v <Bslash>aoReturnCode<Esc>o{<Esc>oINDEP_MSS_BEGIN(ReturnCode);<Esc>oINDEP_MSS_END();<Esc>o}<Esc>kkkk<Bslash>aA 
	else
		" MSS
		map -n <Bslash>ajOMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc><Bslash>ak$
		map -N <Bslash>ajOMSS_BEGIN(<Esc>:call ReadString("ReturnCode")<CR>A);<Esc>oMSS_END();<Esc><Bslash>ak$
		map -m <Bslash>ajOMSS();<Esc><Bslash>ahi
		map -v <Bslash>aoReturnCode<Esc>o{<Esc>oMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc>o}<Esc>kkkk<Bslash>aA 
	endif
endif
