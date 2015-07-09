# Installation #
## Linux ##
 * ln -s $GUBG/vim/ .vim
 * ln -s $GUBG/vim/config.linux.vim .vimrc (.gvimrc gives problems and does
   not load all plugins)
 * cd $GUBG/vim/bundle/YouCompleteMe
 * ./install.sh --clang-completer
## Windows ##
 * Append "source $GUBG/vim/config.windows.vim" to Vim/_vimrc
