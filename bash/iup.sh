export GUBG_IUP=$GUBG_SDKS/iup
kv=`uname -r`
dir=Linux${kv:0:1}${kv:2:1}_64
if [ -d $GUBG_IUP/lib/$dir ];then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GUBG_IUP/lib/$dir
    echo "* Found iup"
fi
export GUBG_CD=$GUBG_SDKS/cd
if [ -d $GUBG_CD/lib/$dir ];then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GUBG_CD/lib/$dir
    echo "* Found cd"
fi
