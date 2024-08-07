#include "s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_equal(big_1, big_2);
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_not_equal(big_1, big_2);
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_greater(big_1, big_2);
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_greater_or_equal(big_1, big_2);
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_less(big_1, big_2);
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_less_or_equal(big_1, big_2);
}
