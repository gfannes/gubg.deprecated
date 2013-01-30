GUBG_ARDUINO = $(GUBG_SDKS)/Arduino

arduino_tgz = $(GUBG_TMP)/arduino.tar.gz
$(arduino_tgz): | env
	wget http://arduino.googlecode.com/files/arduino-1.0.3-src.tar.gz -O $(arduino_tgz)
$(GUBG_ARDUINO): $(arduino_tgz)
	tar xzf $(arduino_tgz) -C $(GUBG_SDKS)

.PHONY: arduino
arduino: $(GUBG_ARDUINO)
