" Ruby mappings
map -p oputs("")<Esc>h==i
map -e A.each do <Bar><Esc>==:call ReadString("Parameter")<CR>A<Bar><Esc>oend<Esc>==O
map -E A.each_with_index do <Bar><Esc>==:call ReadString("Parameter")<CR>A, ix<Bar><Esc>oend<Esc>==O
