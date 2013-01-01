.PHONY: help
help:
	@echo "Help"

.PHONY: clean
clean: 
	ruby internal/cleanAll.rb
	make clean -C cpp/apps/da

da:
	make build -C cpp/apps/da -j

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
