.PHONY: prog test
prog:
	da -E fff.cpp -x
	./fff.exe test.lua

test:
	da -e test/Value_tests.cpp -x
	test/Value_tests.exe
