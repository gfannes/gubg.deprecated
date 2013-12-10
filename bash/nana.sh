export GUBG_NANA=$GUBG_SDKS/nana
if [ -d $GUBG_NANA ];then
    export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$GUBG_NANA/Nana.Cpp11/include/
    export LD_LIBRARY_PATH=$GUBG_NANA/build/bin/:$LD_LIBRARY_PATH
    echo "* Found nana"
fi
