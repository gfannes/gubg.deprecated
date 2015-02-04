#xmodmap $HOME/.xmodmap

alias ls="ls --color "
alias l="find ./ -name "
alias mymount="sudo mount -o rw,noauto,async,user,umask=1000 "
alias myumount="sudo umount "

PS1="\[\033[1;32m\]\u\[\033[1;30m\]@\[\033[1;34m\]\h \[\033[1;34m\]\W\[\033[1;30m\]>\[\033[0m\] "

export LS_COLORS="no=00:fi=00:di=01;34:ln=01;36:pi=40;33:so=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arj=01;31:*.taz=01;31:*.lzh=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.gz=01;31:*.bz2=01;31:*.deb=01;31:*.rpm=01;31:*.jpg=01;35:*.png=01;35:*.gif=01;35:*.bmp=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.png=01;35:*.mpg=01;35:*.avi=01;35:*.fli=01;35:*.gl=01;35:*.dl=01;35:"

export ICUI=$HOME/icui

export GIT_EXTERNAL_DIFF=$GUBG/bin/git_diff.sh
export CSCOPE_DB=$GUBG/cscope.out

export PATH=$PATH:$HOME/.gem/ruby/2.0.0/bin
