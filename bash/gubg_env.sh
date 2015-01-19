if [ ! $GUBG ];then
    echo "ERROR::You have to specify GUBG manually"
    exit 1
fi
if [ ! $GUBG_BUILD ];then
    export GUBG_BUILD=$GUBG/build
    echo "* Setting GUBG_BUILD to $GUBG_BUILD"
fi
if [ ! $GUBG_SDKS ];then
    export GUBG_SDKS=$GUBG_BUILD/sdks
    echo "* Setting GUBG_SDKS to $GUBG_SDKS"
fi
if [ ! $GUBG_TMP ];then
    export GUBG_TMP=$GUBG_BUILD/tmp
    echo "* Setting GUBG_TMP to $GUBG_TMP"
fi
if [ ! $GUBG_BIN ];then
    export GUBG_BIN=$GUBG_BUILD/bin
    echo "* Setting GUBG_BIN to $GUBG_BIN"
fi
export GUBG_BASH=$GUBG/bash
if [ ! $GUBG_NUMBER_CPU ];then
    export GUBG_NUMBER_CPU=`cat /proc/cpuinfo | grep -e "^processor" | wc -l`
fi
export GUBG_PLATFORM=linux
export RUBYLIB=$GUBG/ruby
export PATH=/usr/lib/colorgcc/bin:$PATH:$GUBG_BIN:$GUBG/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
