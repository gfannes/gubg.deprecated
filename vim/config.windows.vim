source $GUBG/vim/config.bram_molenaar.vim

let g:pathogen_disabled = []
call add(g:pathogen_disabled, 'YouCompleteMe')
call add(g:pathogen_disabled, 'rust.vim')
source $GUBG/vim/config.vim

set lines=50 columns=190
set gfn=Courier\ New:h12:cANSI
""set tabstop=4
""set shiftwidth=4
""set noexpandtab
set backupdir=c:/temp
set directory=c:/temp
set tags=w:/tags
let g:ruby_path = "$GUBG/ruby/gubg"
