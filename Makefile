CC = gcc
SRCMODULES = ./decimal/s21_decimal.c
OBJMODULES = $(SRCMODULES:.c=.o)
CFLAGS = -g -Wall -Werror -Wextra -std=c11
LDFLAGS = `pkg-config --cflags --libs check`

TEST_EXEC = run_test.out
REPORT_DIR = ./report

all: s21_decimal.a

test: clean s21_decimal.a
	checkmk clean_mode=1 test/in > test/test.c
	$(CC) $(FLAGS) --coverage -o $(TEST_EXEC) test/test.c $(SRCMODULES) $(LDFLAGS)
	./$(TEST_EXEC)

gcov_report: test
	lcov -t "$(TEST_EXEC)" --output-file report.info --capture -q --directory ./
	genhtml -o $(REPORT_DIR) report.info

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@ 

s21_decimal.a: $(OBJMODULES)
	ar rcs $@ $<

CODE_STYLE = clang-format --style="{CommentPragmas: Insert, BasedOnStyle: Google}"
FILES = find ./ -name '*.c' -print0 -or -name '*.h' -print0

fmt_check:
	$(FILES) | xargs -0 $(CODE_STYLE) -n 

fmt:
	$(FILES) | xargs -0 $(CODE_STYLE) -i

clean:
	find . -name *.o | xargs rm -f
	rm -f *.a *.gcno *.gcda report.info $(TEST_EXEC) test/test.c
	rm -rf $(REPORT_DIR)/

valgrind:
	valgrind --tool=memcheck --leak-check=yes $(TEST_EXEC)

leaks:
	leaks --atExit -- $(TEST_EXEC)
