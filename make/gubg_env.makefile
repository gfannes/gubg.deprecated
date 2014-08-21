ifndef GUBG
$(error You have to specify where gubg is to be found)
endif
ifndef GUBG_SDKS
$(error You have to specify where the sdks are to be found)
endif
ifndef GUBG_TMP
$(error You have to specify where the tmp is to be found)
endif
$(info "GUBG: $(GUBG)")
$(info "GUBG_SDKS: $(GUBG_SDKS)")
$(info "GUBG_TMP: $(GUBG_TMP)")

ifeq ($(GUBG_PLATFORM),win32)
	GUBG_MAKE := make
endif
ifeq ($(GUBG_PLATFORM),linux)
	GUBG_MAKE := make
endif
ifndef GUBG_MAKE
	$(error No make specified)
endif
$(info Using $(GUBG_MAKE))

.PHONY: env
env: $(GUBG_SDKS) $(GUBG_TMP)

$(GUBG_SDKS):;mkdir -p $@
$(GUBG_TMP):;mkdir -p $@
