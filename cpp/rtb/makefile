RTB := $(GUBG)/cpp/rtb
RTB_INCLUDE := $(HOME)/sdks/realtimebattle/include

.PHONY: robots
robots:
	rm -f *.log
	cd robots && da -t $(RTB) -i $(RTB_INCLUDE) -x -e spiral.cpp && mv spiral.exe spiral.robot
	cd robots && da -t $(RTB) -i $(RTB_INCLUDE) -x -e pf1.cpp && mv pf1.exe pf1.robot
