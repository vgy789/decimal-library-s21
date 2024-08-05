CC = gcc
SRCMODULES = ./decimal/alignment.c ./decimal/s21_decimal.c ./decimal/bitwise_helper.c  ./big_decimal/Bcompliment.c ./big_decimal/Balignment.c ./big_decimal/big_decimal.c ./big_decimal/Bbitwise_helper.c
OBJMODULES = $(SRCMODULES:.c=.o)
#CFLAGS = -O2 -flto -Wall -Werror -Wextra -std=c11
CFLAGS = -Wall -Werror -Wextra -std=c11
LDFLAGS = `pkg-config --cflags --libs check`

TEST_EXEC = run_test.out
REPORT_DIR = ./report

all: s21_decimal.a

test: clean s21_decimal.a
	checkmk clean_mode=1 test/in > test/test.c
	$(CC) $(FLAGS) --coverage -o $(TEST_EXEC) test/debug_helper.c test/test.c $(SRCMODULES) $(LDFLAGS)
	./$(TEST_EXEC)

gcov_report: test
	lcov -t "$(TEST_EXEC)" --output-file report.info --capture -q --directory ./
	genhtml -o $(REPORT_DIR) report.info

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@ 

s21_decimal.a: $(OBJMODULES)
	ar rcs $@ $^

CODE_STYLE = clang-format --style="{CommentPragmas: Insert, BasedOnStyle: Google}"
FILES = find ./ -name '*.c' -print0 -or -name '*.h' -print0

fmt_check:
	$(FILES) | xargs -0 $(CODE_STYLE) -n 

fmt:
	$(FILES) | xargs -0 $(CODE_STYLE) -i

clean:
	find . -name "*.out" -or -name "*.o" -or -name "*.gch" -or -name "*.gcno" -or -name "*.gcda" | xargs rm -f
	rm -f report.info $(TEST_EXEC) test/test.c s21_decimal.a
	rm -rf $(REPORT_DIR)/

valgrind:
	valgrind --tool=memcheck --leak-check=yes $(TEST_EXEC)

leaks:
	leaks --atExit -- $(TEST_EXEC)
