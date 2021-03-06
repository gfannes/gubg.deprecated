" Cpp mappings
map -y <Bslash>aa" <<  << "<Esc><Bslash>a4hi

" include ""
map -r <Bslash>aO#include "<Esc>:call ReadString("Header")<CR>A"<Esc>j<Bslash>a
" include <>
map -R <Bslash>aO#include <<Esc>:call ReadString("Header")<CR>A><Esc>j<Bslash>a
" guard
map -g <Bslash>ai#ifndef HEADER_<Esc>:call ReadString("Path to header")<CR>A_ALREADY_INCLUDED<Esc>$by$o#define <Esc>p-o-o-o-oo#endif<Esc>kkk<Bslash>a
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
" TEST_CASE
map -e <Bslash>aoTEST_CASE("", "[]")<Esc><Bslash>a-bkbbla
" SECTION
map -E <Bslash>aoSECTION("")<Esc><Bslash>a-bkbla
" REQUIRE
map -u <Bslash>aoREQUIRE();<Esc><Bslash>aba
" AURO_LOG
map -a <Bslash>aoAURO_LOG_M(<Esc>:call ReadString("Log level")<CR>$bve~ea, );<Esc><Bslash>ahha
" CAP
map -A <Bslash>aaCAP() << <Esc><Bslash>ahhhhha

let hn = hostname()
let athome = 0
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
    " MSS
    map -n <Bslash>ajOMSS_BEGIN(bool);<Esc>oMSS_END();<Esc><Bslash>ak$
    map -N <Bslash>ajOMSS_BEGIN(<Esc>:call ReadString("ReturnCode")<CR>A);<Esc>oMSS_END();<Esc><Bslash>ak$
    map -m <Bslash>ajOMSS();<Esc><Bslash>ahi
    map -v <Bslash>aobool<Esc>o{<Esc>oMSS_BEGIN(bool);<Esc>oMSS_END();<Esc>o}<Esc>kkkk<Bslash>aA 
endif
