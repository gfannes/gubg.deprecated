source $GUBG/vim/colors/darkblue.vim
set smartindent
set tabstop=4
set shiftwidth=4
set expandtab
set lines=50 columns=190
set autochdir
set nowrap

map -p iputs("")<Esc>hi
map -e A.each do <Bar><Bar><Esc>oend<Esc>k$i
map -E A.each_with_index do <Bar>, ix<Bar><Esc>oend<Esc>k$hhhhi
