" Javascript mappings

" block
map -b -o<Bslash>aa{<Esc>-oa}<Esc><Bslash>ak$
" function
map -f <Bslash>aofunction <Esc>:call ReadString("Function name")<CR>A()<Esc>o{<Esc>o};<Esc><Bslash>akk$
" lambda
map -l <Bslash>aafunction(){}<Esc><Bslash>a
" Lambda
map -L <Bslash>aafunction(){}<Esc>i<Enter><Esc><Bslash>ak$
" require
map -r <Bslash>aO<Esc>:call ReadString("Module")<CR>0Davar <Esc>pA = require("<Esc>pA");<Esc><Bslash>aj
" var
map -v <Bslash>ao<Esc>:call ReadString("Var name")<CR>0Davar <Esc>pA = ;<Esc><Bslash>a$i
