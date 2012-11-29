.PHONY: pull commit push upload

pull:
	git pull
commit: pull
	-git commit -a
push: pull commit
	git push
upload: push
	r smeagol
	cd smeagol && scp *.html web-gfannes@fannes.com:fannes.com/www/gubg
webserver: pull
	gollum --config config.rb
	git pull
	git push
