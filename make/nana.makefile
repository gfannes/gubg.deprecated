$(GUBG_NANA): | env
	cd $(GUBG_SDKS) && git clone https://git01.codeplex.com/nanapro
ifeq ($(GUBG_PLATFORM),linux)
	cp $(GUBG_NANA)/Nana.Cpp11/include/nana/config.hpp $(GUBG_NANA)/Nana.Cpp11/include/nana/config.orig.hpp
	sed 's/define USE_NANA_WINDOWS/define USE_NANA_LINUX_X11/' $(GUBG_NANA)/Nana.Cpp11/include/nana/config.orig.hpp > $(GUBG_NANA)/Nana.Cpp11/include/nana/config.hpp
endif

.PHONY: nana
nana: $(GUBG_NANA)
	mkdir $(GUBG_NANA)/Nana.Cpp11/build/bin
	cd $(GUBG_NANA)/Nana.Cpp11/build/makefile && make -j $(GUBG_NUMBER_CPU)
