" Rust mappings

" include ""
map -r <Bslash>aOextern crate <Esc>:call ReadString("Crate")<CR>A;<Esc>j<Bslash>a
" block
map -b -o<Bslash>aa{<Esc>-oa}<Esc><Bslash>ak$
" struct
map -c <Bslash>aostruct <Esc>:call ReadString("Struct name")<CR>-oa{<Esc>-oa}<Esc><Bslash>ak$
" for
map -f <Bslash>aofor (<Esc>:call ReadString("Type")<CR>A::iterator it = <Esc>:call ReadString("Container")<CR>A.begin(); it != .end(); ++it)<Esc>o{<Esc>o}<Esc><Bslash>akhhhhhhh
