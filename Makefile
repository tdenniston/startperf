example: example.c
	$(CC) -std=c99 -o $@ $^ -lm -D_POSIX_SOURCE

check: test.c
	$(CC) -o test $^
	@$(RM) *.testexe
	./test
	@sleep 1
	@test -e test.testexe || { echo Test failed; exit 1; } && echo Test passed.
	./test.testexe
	@sleep 1
	@test -e test.testexe.testexe || { echo Test failed; exit 1; } && echo Test passed.
	@$(RM) *.testexe

clean:
	$(RM) test *.testexe example
