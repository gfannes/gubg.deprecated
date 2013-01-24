export GUBG_BOOST_VERSION=1_51_0
export GUBG_BOOST=$GUBG_SDKS/boost_$GUBG_BOOST_VERSION
if [ -d $GUBG_BOOST ];then
    export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$GUBG_BOOST/include
    echo "* Found boost includes"
fi
if [ -d $GUBG_BOOST/stage/lib ];then
    export LD_LIBRARY_PATH=$GUBG_BOOST/stage/lib:$LD_LIBRARY_PATH
    echo "* Found boost libs"
fi
