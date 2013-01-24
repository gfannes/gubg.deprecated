GUBG_LUA_51 = $(GUBG_SDKS)/lua5.1
GUBG_LUA_52 = $(GUBG_SDKS)/lua52
GUBG_IM = $(GUBG_SDKS)/im
GUBG_CD = $(GUBG_SDKS)/cd
GUBG_IUP = $(GUBG_SDKS)/iup

lua51_tgz = $(GUBG_TMP)/lua51.tar.gz
$(lua51_tgz): | env
	wget http://sourceforge.net/projects/luabinaries/files/5.1.4/Docs%20and%20Sources/lua5_1_4_Sources.tar.gz -O $(lua51_tgz)
$(GUBG_LUA_51): $(lua51_tgz)
	tar xzf $(lua51_tgz) -C $(GUBG_SDKS)

lua52_tgz = $(GUBG_TMP)/lua52.tar.gz
$(lua52_tgz): | env
	wget http://sourceforge.net/projects/luabinaries/files/5.2.1/Docs%20and%20Sources/lua-5.2.1_Sources.tar.gz -O $(lua52_tgz)
$(GUBG_LUA_52): $(lua52_tgz)
	tar xzf $(lua52_tgz) -C $(GUBG_SDKS)

im_tgz = $(GUBG_TMP)/im.tar.gz
$(im_tgz): | env
	wget http://sourceforge.net/projects/imtoolkit/files/3.8.1/Docs%20and%20Sources/im-3.8.1_Sources.tar.gz -O $(im_tgz)
$(GUBG_IM): $(im_tgz)
	tar xzf $(im_tgz) -C $(GUBG_SDKS)

cd_tgz = $(GUBG_TMP)/cd.tar.gz
$(cd_tgz): | env
	wget http://sourceforge.net/projects/canvasdraw/files/5.6/Docs%20and%20Sources/cd-5.6_Sources.tar.gz -O $(cd_tgz)
$(GUBG_CD): $(cd_tgz)
	tar xzf $(cd_tgz) -C $(GUBG_SDKS)

iup_tgz = $(GUBG_TMP)/iup.tar.gz
$(iup_tgz): | env
	wget http://sourceforge.net/projects/iup/files/3.7/Docs%20and%20Sources/iup-3.7_Sources.tar.gz -O $(iup_tgz)
$(GUBG_IUP): $(iup_tgz)
	tar xzf $(iup_tgz) -C $(GUBG_SDKS)

.PHONY: lua im cd iup clean_iup
lua: $(GUBG_LUA_51) $(GUBG_LUA_52)
	cd $(GUBG_LUA_51)/src && make
	cd $(GUBG_LUA_52)/src && make
im: $(GUBG_IM) lua
	cd $(GUBG_IM)/src && make
cd: $(GUBG_CD) im lua
	cd $(GUBG_CD)/src && make
iup: $(GUBG_IUP) cd im lua
	cd $(GUBG_IUP)/src && make
iup-clean:
#	rm -f $(lua51_tgz) $(lua52_tgz) $(im_tgz) $(cd_tgz) $(iup_tgz)
	rm -rf $(GUBG_LUA_51)
	rm -rf $(GUBG_LUA_52)
	rm -rf $(GUBG_IM)
	rm -rf $(GUBG_CD)
	rm -rf $(GUBG_IUP)
