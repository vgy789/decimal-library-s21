CC		  = gcc -std=c11
CFLAGS	  = -Wall -Werror -Wextra -pedantic
OPTFLAGS  = -O2 -flto
CFLAGS	 += $(OPTFLAGS)
LDFLAGS	  = `pkg-config --cflags --libs check`

SRCMODULES  = \
	./decimal/arithmetic.c \
	./decimal/comparison.c \
	./decimal/converter.c \
	./decimal/rounding.c \
	./decimal/various.c \
	./big_decimal/Balignment.c \
	./big_decimal/Barithmetic.c \
	./big_decimal/Bcomparison.c \
	./big_decimal/Brounding.c \
	./big_decimal/Bvarious.c

OBJMODULES  = $(SRCMODULES:.c=.o)

TEST_EXEC   = run_test.out
REPORT_DIR  = ./report

all: s21_decimal.a		# Bbuilds the library

rebuild: clean all		# Cleans and builds the library

s21_decimal.a: $(OBJMODULES)
	ar rcs $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

test: clean s21_decimal.a		# Generates test file, builds with coverage, and runs tests
	@checkmk clean_mode=1 test/in > test/test.c && make fmt
	$(CC) $(CFLAGS) --coverage -o $(TEST_EXEC) test/debug_helper.c test/test.c $(SRCMODULES) $(LDFLAGS)
	./$(TEST_EXEC)


gcov_report: test		# Generate gcov report from the tests
	lcov -c -t "$(TEST_EXEC)" --directory ./ -o report.info --exclude *debug_helper.c --exclude *test.c
	genhtml -o $(REPORT_DIR) report.info


clean:		# Clean build artifacts
	find . -name "*.out" -or -name "*.o" -or -name "*.gch" -or -name "*.gcno" -or -name "*.gcda" | xargs rm -f
	rm -f report.info $(TEST_EXEC) s21_decimal.a
	rm -rf $(REPORT_DIR)/


CODE_STYLE   = clang-format --style="{CommentPragmas: Insert, BasedOnStyle: Google}"
FMT_FILES	= find ./ -name '*.c' -print0 -or -name '*.h' -print0

fmt_check:		# Code formatting check
	$(FMT_FILES) | xargs -0 $(CODE_STYLE) -n

fmt:		# Code formatting
	$(FMT_FILES) | xargs -0 $(CODE_STYLE) -i

# Targets for memory checking under different systems

valgrind:		# Linux: perform valgrind memory check
	sed -i 's/CK_FORK/CK_NOFORK/g' test/test.c
	$(MAKE) test
	sed -i 's/CK_NOFORK/CK_FORK/g' test/test.c
	valgrind --tool=memcheck --leak-check=yes ./$(TEST_EXEC)

leaks:		# macOS/BSD: perform leaks check
	sed -i '' 's/CK_FORK/CK_NOFORK/g' test/test.c
	$(MAKE) test
	sed -i '' 's/CK_NOFORK/CK_FORK/g' test/test.c
	leaks --atExit -- ./$(TEST_EXEC)

cppcheck:		# Run cppcheck for static analysis
	cppcheck --enable=all --suppress=missingIncludeSystem .

help:		## Display this help screen
	@grep -h -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
	awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
