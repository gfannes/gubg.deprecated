
.PHONY: gubg
gubg:
	rustc gubg.rs --lib

.PHONY: tests
tests: gubg
	rustc test/cartouche_tests.rs -o test/cartouche_tests.exe -L .
	test/cartouche_tests.exe
