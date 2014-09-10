" Javascript mappings

" block
map -b -o<Bslash>aa{<Esc>-oa}<Esc><Bslash>ak$
" function
map -f <Bslash>aofunction <Esc>:call ReadString("Function name")<CR>A()<Esc>o{<Esc>o};<Esc><Bslash>akk$
" lambda
map -l <Bslash>aafunction(){}<Esc><Bslash>a
" Lambda
map -L <Bslash>aafunction()<Esc>o{<Esc>o};<Esc><Bslash>akk$
