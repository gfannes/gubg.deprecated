if [ ! $GUBG ];then
    echo "ERROR::You have to specify GUBG manually"
    exit 1
fi
if [ ! $GUBG_SDKS ];then
    export GUBG_SDKS=$HOME/sdks
    echo "* Setting GUBG_SDKS to $GUBG_SDKS"
fi
if [ ! $GUBG_TMP ];then
    export GUBG_TMP=$HOME/tmp/gubg
    echo "* Setting GUBG_TMP to $GUBG_TMP"
fi
if [ ! $GUBG_BIN ];then
    export GUBG_BIN=$HOME/bin
    echo "* Setting GUBG_BIN to $GUBG_BIN"
fi
export GUBG_BASH=$GUBG/bash
export GUBG_NUMBER_CPU=`cat /proc/cpuinfo | grep processor | wc -l`
export RUBYLIB=$GUBG/ruby
export PATH=$PATH:$GUBG_BIN:$GUBG/bin
