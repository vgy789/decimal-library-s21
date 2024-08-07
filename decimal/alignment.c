#include "s21_decimal.h"

// почти не используются в s21_decimal, но используются в тестах
// перенеси функции в debug_helper

void alignment(s21_decimal *value_1, s21_decimal *value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(*value_1, &big_1);
  decimal_to_big(*value_2, &big_2);
  Balignment(&big_1, &big_2);
  big_to_decimal(big_1, value_1);
  big_to_decimal(big_2, value_2);
}

void circumcision(s21_decimal *value) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bcircumcision(&big);
  big_to_decimal(big, value);
}
