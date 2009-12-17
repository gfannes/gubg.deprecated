" source $GUBG/vim/colors/darkblue.vim
source $GUBG/vim/colors/ir_black.vim
set smartindent
set tabstop=4
set shiftwidth=4
set expandtab
set lines=50 columns=190
set autochdir
set nowrap
set visualbell

function! ReadString(message)
  let curline = getline('.')
  call inputsave()
  let name = input(a:message . ': ')
  call inputrestore()
  call setline('.', curline . name)
endfunction
map _o :a<CR><CR>.<CR>

autocmd bufenter *.rb source $GUBG/vim/maps.ruby.vim
autocmd bufenter *.lua source $GUBG/vim/maps.lua.vim
autocmd bufenter *.cpp source $GUBG/vim/maps.cpp.vim
autocmd bufenter *.h source $GUBG/vim/maps.cpp.vim
autocmd bufenter *.txt source $GUBG/vim/maps.markdown.vim

source $GUBG/vim/hexmode.vim

command -bar Q q
