echo "== Setting environment variables"
. $GUBG/bash/gubg_env.sh
echo "== Configuring packages"
. $GUBG_BASH/boost.sh
. $GUBG_BASH/iup.sh
. $GUBG_BASH/arduino.sh
. $GUBG_BASH/nana.sh
. $GUBG_BASH/decode-it.sh
echo "== Loading personal settings"
. $GUBG_BASH/personal.$USER.sh
#alias g="gvim -u $GUBG/vim/config.linux.vim --remote-tab-silent "
#alias g="gvim --remote-tab-silent "
export SDKS=$HOME/sdks
export DSRC=$HOME/d/src
export BULLET=$HOME/bullet
export BLAZE=$SDKS/blaze
export SFML=$SDKS/SFML
export WT=$SDKS/wt/build/src:$SDKS/wt/build/src/http
export LUA_INIT=@/home/gfannes/config.lua
export ROOTSYS=$HOME/sdks/root
#export PATH=$HOME/bin:$GUBG/bin:$HOME/d/dmd2/linux/bin:$HOME/software/bin:/usr/local/bin:$PATH:$ROOTSYS/bin
#export LD_LIBRARY_PATH=$HOME/software/lib:$BOOST_HOME/stage/lib:/usr/local/lib:$SFML/lib:$WT:$ROOTSYS/lib:$IUP/lib/Linux35_64:$CD/lib/Linux35_64
export LIBRARY_PATH=$HOME/software/lib:$SFML/lib
export C_INCLUDE_PATH=$HOME/software/include
#export CPLUS_INCLUDE_PATH=$HOME/software/include:$BOOST_HOME/include
export PKG_CONFIG_PATH=$HOME/software/lib/pkgconfig
export EMACSRC=$HOME/gubg/emacs
export INSTIKI=$HOME/software/instiki-0.13.0
