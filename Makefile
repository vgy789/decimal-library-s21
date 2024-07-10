CC = gcc
SRCMODULES = big_decimal.c s21_decimal.c
OBJMODULES = $(SRCMODULES:.c=.o)
CFLAGS = -g -Wall -Werror -Wextra -std=c11
LDFLAGS = `pkg-config --cflags --libs check`

TEST_EXEC = run_test
REPORT_DIR = ./report

all: s21_decimal.a

test: clean s21_decimal.a
	checkmk test/in > test/test.c
	$(CC) $(FLAGS) --coverage -o $(TEST_EXEC) test/test.c $(SRCMODULES) $(LDFLAGS)
	./$(TEST_EXEC)

gcov_report: test
	lcov -t "$(TEST_EXEC)" --output-file report.info --capture -q --directory ./
	genhtml -o $(REPORT_DIR) report.info

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@ 

s21_decimal.a: s21_decimal.o $(OBJMODULES)
	ar rcs $@ $<

FORMAT = clang-format --style="{CommentPragmas: Insert, BasedOnStyle: Google}"

format_check:
	find ./ -name '*.c' -or -name '*.h' -print0 | xargs -0 $(FORMAT) -n 

format:
	find ./ -name '*.c' -or -name '*.h' -print0 | xargs -0 $(FORMAT) -i

clean:
	rm -f *.o *.a *.gcno *.gcda report.info $(TEST_EXEC) test/test.c
	rm -rf $(REPORT_DIR)/ 

valgrind:
	valgrind --tool=memcheck --leak-check=yes $(TEST_EXEC)

leaks:
	leaks --atExit -- $(TEST_EXEC)