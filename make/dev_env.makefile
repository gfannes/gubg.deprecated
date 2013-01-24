.PHONY: dev_env
dev_env:
	sudo apt-get install -y build-essential
	sudo apt-get install -y libreadline-dev
	sudo apt-get install -y gcc-avr
	sudo apt-get install -y avr-libc
	sudo apt-get install -y avrdude
	sudo apt-get install -y libxmu-dev
	sudo usermod -a -G dialout $(USER)
