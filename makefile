.PHONY: help
help:
	@echo "Help"

include make/gubg_env.makefile
include make/dev_env.makefile
include make/boost.makefile
include make/iup.makefile
include make/da.makefile

.PHONY: clean
clean: da-clean
	ruby internal/cleanAll.rb

.PHONY: fix
fix:
	cd cpp/libs/gubg && da -f guards -r
	cd cpp/libs/iup && da -f guards -r
	cd cpp/apps/da && da -f guards -r

.PHONY: pull commit publish upload
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
