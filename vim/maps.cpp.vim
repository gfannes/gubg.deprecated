" Cpp mappings
map -l <Bslash>aiSTREAM()<Esc><Bslash>ai
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
" namespace
map -s <Bslash>aonamespace <Esc>:call ReadString("Namespace name")<CR>-oa{<Esc>-oa}<Esc><Bslash>ak$

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
" MSS
map -n <Bslash>ajOMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc><Bslash>ak$
map -N <Bslash>ajOMSS_BEGIN(<Esc>:call ReadString("ReturnCode")<CR>A);<Esc>oMSS_END();<Esc><Bslash>ak$
map -m <Bslash>ajOMSS();<Esc><Bslash>ahi
map -v <Bslash>aoReturnCode<Esc>o{<Esc>oMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc>o}<Esc>kkkk<Bslash>aA 
else
""" MSS
""map -n <Bslash>ajOMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc><Bslash>ak$
""map -N <Bslash>ajOMSS_BEGIN(<Esc>:call ReadString("ReturnCode")<CR>A);<Esc>oMSS_END();<Esc><Bslash>ak$
""map -m <Bslash>ajOMSS();<Esc><Bslash>ahi
""map -v <Bslash>aoReturnCode<Esc>o{<Esc>oMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc>o}<Esc>kkkk<Bslash>aA 
" INDEP_MSS
map -n <Bslash>ajOINDEP_MSS_BEGIN(ReturnCode);<Esc>oINDEP_MSS_END();<Esc><Bslash>ak$
map -N <Bslash>ajOINDEP_MSS_BEGIN(<Esc>:call ReadString("ReturnCode")<CR>A);<Esc>oINDEP_MSS_END();<Esc><Bslash>ak$
map -m <Bslash>ajOINDEP_MSS();<Esc><Bslash>ahi
map -v <Bslash>aoReturnCode<Esc>o{<Esc>oINDEP_MSS_BEGIN(ReturnCode);<Esc>oINDEP_MSS_END();<Esc>o}<Esc>kkkk<Bslash>aA 
endif
