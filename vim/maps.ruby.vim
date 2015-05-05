" Ruby mappings
map -p oputs("")<Esc>==$hi
map -d A do <Bar><Esc>==:call ReadString("Parameter")<CR>A<Bar><Esc>oend<Esc>==O
map -e A.each do <Bar><Esc>==:call ReadString("Parameter")<CR>A<Bar><Esc>oend<Esc>==O
map -E A.each_with_index do <Bar><Esc>==:call ReadString("Parameter")<CR>A, ix<Bar><Esc>oend<Esc>==O
map -r <Bslash>aArequire('<Esc>:call ReadString("Module")<CR>A')<Esc><Bslash>ao<Esc>
map -y a#{}<Esc>i
map -c _oIclass <Esc>:call ReadString("Class")<CR>==oend<Esc>k$
map -f _oAdef <Esc>:call ReadString("Function")<CR>==oend<Esc>k$
