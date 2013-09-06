.PHONY: help clean fix da
.PHONY: pull commit publish upload
help:
	@echo "help clean fix da boost arduino"

include make/gubg_env.makefile
include make/dev_env.makefile
include make/boost.makefile
include make/nana.makefile
include make/iup.makefile
include make/arduino.makefile

clean:
	ruby internal/cleanAll.rb
	$(GUBG_MAKE) da-clean -f make/da.makefile

fix:
	cd cpp/libs/gubg && da -f guards -r
	cd cpp/libs/garf && da -f guards -r
	cd cpp/libs/iup && da -f guards -r
	cd cpp/apps/da && da -f guards -r

da:
	$(GUBG_MAKE) da -f make/da.makefile -j $(GUBG_NUMBER_CPU)

pull:
	git pull
commit: pull
	-git commit -a
publish: pull commit
	git push
	cd ../Dropbox/gubg && git pull ../../gubg
upload: push
	r smeagol
	cd smeagol && scp *.html web-gfannes@fannes.com:fannes.com/www/gubg
webserver: pull
	gollum --config config.rb
	git pull
	git push
