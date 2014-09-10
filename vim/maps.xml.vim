" XML mappings

" inline tag
map -t <Bslash>amm-o<Esc>:call ReadString("Tag")<CR>0Da<></><Esc>hhhPlllp^d00D`mpmm<Bslash>ajdd`mF>
" tag on two new lines
map -T <Bslash>ao<<Esc>:call ReadString("Tag")<CR>A><Esc>T<yt>o</><Esc>P==k<Bslash>a$
" tag on one new line
map -n <Bslash>ao<<Esc>:call ReadString("Tag")<CR>A><Esc>T<yt>A</><Esc>PF<<Bslash>ai

" attribute
map -a <Bslash>amm-o<Esc>:call ReadString("Attribute")<CR>0"aDdd`mi <Esc>"apa=""<Esc><Bslash>ai
