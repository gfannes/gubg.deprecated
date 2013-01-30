export GUBG_ARDUINO_VERSION=1.0.3
export GUBG_ARDUINO=$GUBG_SDKS/arduino-$GUBG_ARDUINO_VERSION
if [ -d $GUBG_ARDUINO ];then
    echo "* Found arduino"
fi
