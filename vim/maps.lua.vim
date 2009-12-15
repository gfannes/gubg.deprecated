" Lua mappings
map -p oprint("")<Esc>hi
map -r Arequire("<Esc>:call ReadString("Module")<CR>A")<Esc>o<Esc>
map -y a" ..  .. "<Esc>hhhhi
map -e A.each do <Bar><Esc>:call ReadString("Parameter")<CR>A<Bar><Esc>oend<Esc>O
map -E A.each_with_index do <Bar><Esc>:call ReadString("Parameter")<CR>A, ix<Bar><Esc>oend<Esc>O
