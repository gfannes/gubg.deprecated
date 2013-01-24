BOOST_TBZ = $(GUBG_TMP)/$(GUBG_BOOST_VERSION).tar.bz2
$(BOOST_TBZ): | env
	wget http://sourceforge.net/projects/boost/files/boost/$(subst _,.,$(GUBG_BOOST_VERSION))/boost_$(GUBG_BOOST_VERSION).tar.bz2 -O $(BOOST_TBZ)
$(GUBG_BOOST): $(BOOST_TBZ)
	tar xjf $(BOOST_TBZ) -C $(GUBG_SDKS)

.PHONY: boost
boost: $(GUBG_BOOST)
	cd $(GUBG_BOOST) && ./bootstrap.sh
	cd $(GUBG_BOOST) && ./b2
