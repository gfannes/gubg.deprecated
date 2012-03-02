" Cpp mappings
map -p <Bslash>aoLOG_M("");<Esc><Bslash>ahhhi
map -P <Bslash>aoLOG_M_(<Esc>:call ReadString("Log level")<CR>A, "");<Esc><Bslash>ahhhi
map -y <Bslash>aa" <<  << "<Esc><Bslash>a4hi
map -l o<Esc>:call ReadString("Line")<CR>$

" include
map -r <Bslash>ajO#include "<Esc>:call ReadString("Header")<CR>A"<Esc><Bslash>a
" guard
map -g <Bslash>ai#ifndef <Esc>:call ReadString("Path to header")<CR>$by$o#define <Esc>p-o-oo#endif<Esc>kk<Bslash>a
" block
map -b -o<Bslash>aa{<Esc>-oa}<Esc><Bslash>ak$
" class
map -c <Bslash>aoclass <Esc>:call ReadString("Class name")<CR>-oa{<Esc>-oapublic:<Esc>-oa};<Esc><Bslash>ak$
" MSS
map -n <Bslash>ajOMSS_BEGIN(ReturnCode);<Esc>oMSS_END();<Esc><Bslash>ak$
map -N <Bslash>ajOMSS_BEGIN(<Esc>:call ReadString("ReturnCode")<CR>A);<Esc>oMSS_END();<Esc><Bslash>ak$
map -m <Bslash>ajOMSS<Esc>:call ReadString("Suffix")<CR>A();<Esc><Bslash>ahi
