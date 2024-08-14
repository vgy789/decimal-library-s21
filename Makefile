CC = gcc
SRCMODULES = ./decimal/arithmetic.c ./decimal/comparison.c ./decimal/converter.c ./decimal/rounding.c ./decimal/various.c ./big_decimal/Balignment.c ./big_decimal/Barithmetic.c ./big_decimal/Bcomparison.c ./big_decimal/Brounding.c ./big_decimal/Bvarious.c
OBJMODULES = $(SRCMODULES:.c=.o)
CFLAGS = -Wall -Werror -Wextra -std=c11 -Wunused-function -pedantic
OPTFLAGS = -O2 -flto
LDFLAGS = `pkg-config --cflags --libs check`

TEST_EXEC = run_test.out
REPORT_DIR = ./report

all: s21_decimal.a

rebuild: clean s21_decimal.a

test: clean s21_decimal.a
	# checkmk clean_mode=1 test/in > test/test.c
	$(CC) $(CFLAGS) --coverage -o $(TEST_EXEC) test/debug_helper.c test/test.c $(SRCMODULES) $(LDFLAGS)
	./$(TEST_EXEC)

gcov_report: test
	lcov -c -t "$(TEST_EXEC)" --directory ./ -o report.info --exclude *debug_helper.c --exclude *test.c
	genhtml -o $(REPORT_DIR) report.info

%.o: %.c %.h
	$(CC) $(CFLAGS) $(OPTFLAGS) -c $< -o $@ 

s21_decimal.a: $(OBJMODULES)
	ar rcs $@ $^

clean:
	find . -name "*.out" -or -name "*.o" -or -name "*.gch" -or -name "*.gcno" -or -name "*.gcda" | xargs rm -f
	rm -f report.info $(TEST_EXEC) s21_decimal.a
	rm -rf $(REPORT_DIR)/


fmt_check fmt: CODE_STYLE = clang-format --style="{CommentPragmas: Insert, BasedOnStyle: Google}"
fmt_check fmt: FMT_FILES = find ./ -name '*.c' -print0 -or -name '*.h' -print0

fmt_check:
	$(FMT_FILES) | xargs -0 $(CODE_STYLE) -n 

fmt:
	$(FMT_FILES) | xargs -0 $(CODE_STYLE) -i


valgrind leaks: OPTFLAGS =
valgrind leaks: CFLAGS += -g

valgrind:
	sed -i 's/CK_FORK/CK_NOFORK/g' test/test.c
	make test; \
	sed -i 's/CK_NOFORK/CK_FORK/g' test/test.c
	valgrind --tool=memcheck --leak-check=yes ./$(TEST_EXEC)

leaks:
	sed -i 's/CK_FORK/CK_NOFORK/g' test/test.c
	make test; \
	sed -i 's/CK_NOFORK/CK_FORK/g' test/test.c
	leaks --atExit -- ./$(TEST_EXEC)

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem .
