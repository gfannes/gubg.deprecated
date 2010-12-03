"source $GUBG/vim/colors/darkblue.vim
"source $GUBG/vim/colors/ir_black.vim
colorscheme desert
set smartindent
set tabstop=4
set shiftwidth=4
set expandtab
set autochdir
set wrap
set visualbell
set scroll=1
"Remove the 3000 character syntax highlighting limit
set synmaxcol=0

function! ReadString(message)
  let curline = getline('.')
  call inputsave()
  let name = input(a:message . ': ')
  call inputrestore()
  call setline('.', curline . name)
endfunction
map _o :a<CR><CR>.<CR>
map -o :a<CR><CR>.<CR>

function! InsertOneChar()
    let c = nr2char(getchar())
    let i = 0
    while i < v:count1
        :exec "normal i".c."\el"
        let i += 1
    endwhile
endfunction
command -count InsertOneCharCmd call InsertOneChar()
map <Space> :InsertOneCharCmd<CR>

command LongLines /\%>110v.\+

autocmd BufEnter,BufNewFile,BufRead *.rb source $GUBG/vim/maps.ruby.vim
autocmd BufEnter,BufNewFile,BufRead *.lua source $GUBG/vim/maps.lua.vim
autocmd BufEnter,BufNewFile,BufRead *.d source $GUBG/vim/maps.d.vim
autocmd BufEnter,BufNewFile,BufRead *.cpp source $GUBG/vim/maps.cpp.vim
autocmd BufEnter,BufNewFile,BufRead *.h source $GUBG/vim/maps.cpp.vim
autocmd BufEnter,BufNewFile,BufRead *.txt source $GUBG/vim/maps.markdown.vim

source $GUBG/vim/hexmode.vim

command -bar Q q

source $GUBG/vim/tabnumber.vim
source $GUBG/vim/autoclose.vim
