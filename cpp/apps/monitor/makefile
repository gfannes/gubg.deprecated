.PHONY: doitall fix
doitall: $(GUBG_BIN)
	da -e monitor.cpp
ifeq ($(GUBG_PLATFORM),linux)
	cp monitor.exe $(GUBG_BIN)/monitor
endif
ifeq ($(GUBG_PLATFORM),win32)
	cp monitor.exe $(GUBG_BIN)
endif

fix:
	da -f guards -r
