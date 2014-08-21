"source $GUBG/vim/colors/darkblue.vim
"source $GUBG/vim/colors/ir_black.vim
colorscheme desert
set smartindent
set tabstop=4
set shiftwidth=4
set expandtab
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
autocmd BufEnter,BufNewFile,BufRead *.hpp source $GUBG/vim/maps.cpp.vim
autocmd BufEnter,BufNewFile,BufRead *.inc source $GUBG/vim/maps.cpp.vim
autocmd BufEnter,BufNewFile,BufRead *.js source $GUBG/vim/maps.cpp.vim
autocmd BufEnter,BufNewFile,BufRead *.jscad source $GUBG/vim/maps.cpp.vim
""autocmd BufEnter,BufNewFile,BufRead *.rs source $GUBG/vim/maps.cpp.vim
""autocmd BufEnter,BufNewFile,BufRead *.rs source $GUBG/vim/rust.vim
autocmd BufEnter,BufNewFile,BufRead *.txt source $GUBG/vim/maps.markdown.vim
autocmd BufEnter,BufNewFile,BufRead *.json source $GUBG/vim/json.vim
autocmd BufEnter,BufNewFile,BufRead *.asciidoc source $GUBG/vim/asciidoc2.vim
autocmd BufEnter,BufNewFile,BufRead *.adoc source $GUBG/vim/asciidoc2.vim
autocmd BufEnter,BufNewFile,BufRead *.puml source $GUBG/vim/plantuml.vim
autocmd BufEnter,BufNewFile,BufRead *.tjp source $GUBG/vim/tjp.vim
autocmd BufEnter,BufNewFile,BufRead *.tji source $GUBG/vim/tjp.vim
"au! BufRead,BufNewFile *.json set filetype=json foldmethod=syntax 

source $GUBG/vim/hexmode.vim

command -bar Q q

source $GUBG/vim/tabnumber.vim
source $GUBG/vim/autoclose.vim
source $GUBG/vim/cscope_maps.vim
source $GUBG/vim/git.vim
""source $GUBG/vim/rust.vim

:au FocusLost * silent! wa

set ignorecase
set smartcase
