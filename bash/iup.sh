export GUBG_IUP=$GUBG_SDKS/iup
if [ -d $GUBG_IUP/lib/Linux35_64 ];then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GUBG_IUP/lib/Linux35_64
    echo "* Found iup"
fi
export GUBG_CD=$GUBG_SDKS/cd
if [ -d $GUBG_CD/lib/Linux35_64 ];then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GUBG_CD/lib/Linux35_64
    echo "* Found cd"
fi
