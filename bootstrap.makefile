#This is the gubg bootstrap file

GUBG := $(HOME)/gubg
BASHRC := $(HOME)/.bashrc

.PHONY: help
help:
	@echo "HELP:"
	@echo "install-gubg: clones the gubg repo $(GUBG) and installs its bash settings into $(BASHRC)"

#Necessary tools
GIT := $(shell which git)
ifeq ($(GIT),)
$(error "Git could not be found, please install it: sudo apt-get install git")
endif

.PHONY: install-gubg
install-gubg: $(GUBG) adjust-bashrc

#Installs loading the bash.cfg from gubg into .bashrc
.PHONY: adjust-bashrc
ifeq ($(shell grep "export GUBG=" $(BASHRC)),)
adjust-bashrc:
	echo "export GUBG=$(GUBG)" >> $(BASHRC)
	echo ". $(GUBG)/bash/bash.sh" >> $(BASHRC)
else
adjust-bashrc:
	@echo "GUBG is already installed into $(BASHRC)"
endif

#Clones the gubg repo
$(GUBG):
	$(GIT) clone https://github.com/gfannes/gubg.git $(GUBG)
