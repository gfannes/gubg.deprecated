.PHONY: doitall fix
doitall: $(GUBG_BIN)
	da -e pa.cpp
ifeq ($(GUBG_PLATFORM),linux)
	cp pa.exe $(GUBG_BIN)/pa
endif
ifeq ($(GUBG_PLATFORM),win32)
	cp pa.exe $(GUBG_BIN)
endif

fix:
	da -f guards -r
