"source $GUBG/vim/colors/darkblue.vim
"source $GUBG/vim/colors/ir_black.vim
set smartindent
set tabstop=4
set shiftwidth=4
set expandtab
set lines=43 columns=190
set autochdir
set nowrap
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

autocmd bufenter *.rb source $GUBG/vim/maps.ruby.vim
autocmd bufenter *.lua source $GUBG/vim/maps.lua.vim
autocmd bufenter *.d source $GUBG/vim/maps.d.vim
autocmd bufenter *.cpp source $GUBG/vim/maps.cpp.vim
autocmd bufenter *.h source $GUBG/vim/maps.cpp.vim
autocmd bufenter *.txt source $GUBG/vim/maps.markdown.vim

source $GUBG/vim/hexmode.vim

command -bar Q q
