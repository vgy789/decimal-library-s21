SRCMODULES = ???.c
OBJMODULES = $(SRCMODULES:.c=.o)
CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TEST_EXEC = test_s21_decimal
LIBS = `pkg-config --cflags --libs check`

all: s21_decimal.a

test:
	$(CC) $(FLAGS) --coverage -lm -o $(TEST_EXEC) tests/*.c tests/test_suites/*.c s21_decimal.c $(LIBS)
	./$(TEST_EXEC)

gcov_report: test
	lcov -t "$(TEST_EXEC)" --output-file report.info --capture -q --directory ./
	genhtml -o $(REPORT_DIR) report.info

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@ 

s21_decimal.a: s21_decimal.o $(OBJMODULES)
	ar rcs $@ $<

FORMAT = clang-format --style="{BasedOnStyle: Google}"

cf_check:
	find ./ -name '*.c' -or -name '*.h' -print0 | xargs -0 $(FORMAT) -n 

cf_apply:
	find ./ -name '*.c' -or -name '*.h' -print0 | xargs -0 $(FORMAT) -i

clean:
	rm -f *.o *.a *.gcno *.gcda $(REPORT_DIR)/ report.info $(TEST_EXEC)

valgrind:
	valgrind --tool=memcheck --leak-check=yes  ./test_s21_decimal

leaks:
	leaks --atExit -- ./test_s21_decimal