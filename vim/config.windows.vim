source $GUBG/vim/config.vim
if hostname() ==? "GET11029"
	set lines=62 columns=210
	set gfn=Courier:h12:cANSI
else
	set lines=50 columns=190
	set gfn=Courier\ New:h12:cANSI
endif
set tabstop=4
set shiftwidth=4
set noexpandtab
set backupdir=c:/temp
set directory=c:/temp
set tags=w:/tags
let g:ruby_path = "$GUBG/ruby/gubg"
