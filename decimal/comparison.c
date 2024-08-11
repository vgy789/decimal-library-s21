#include "../big_decimal/big_decimal.h"
#include "s21_decimal.h"

typedef int (*compare_func)(big_decimal, big_decimal);

static int compare_decimals(s21_decimal value_1, s21_decimal value_2,
                            compare_func cmp) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return cmp(big_1, big_2);
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  return compare_decimals(value_1, value_2, Bs21_is_equal);
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  return compare_decimals(value_1, value_2, Bs21_is_not_equal);
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  return compare_decimals(value_1, value_2, Bs21_is_greater);
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return compare_decimals(value_1, value_2, Bs21_is_greater_or_equal);
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return compare_decimals(value_1, value_2, Bs21_is_less);
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return compare_decimals(value_1, value_2, Bs21_is_less_or_equal);
}
