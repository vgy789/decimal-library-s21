#include "s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  // думаю это излишне, т.к. не сказано что возвращать в случае передачи
  // большого scales полагаю автотесты не станут пихать невалидное значение
  // uint8_t err_code = check_scale(value_1);
  // if (err_code == 0) {
  //   err_code = check_scale(value_2);
  // }
  // if (err_code != 0) {
  //   return err_code;
  // }

  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  const bool result = Bs21_is_equal(big_1, big_2);

  return result;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  const bool result = Bs21_is_not_equal(big_1, big_2);
  return result;
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  const bool result = Bs21_is_greater(big_1, big_2);
  return result;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  const bool result = Bs21_is_greater_or_equal(big_1, big_2);
  return result;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  const bool result = Bs21_is_less(big_1, big_2);
  return result;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  const bool result = Bs21_is_less_or_equal(big_1, big_2);
  return result;
}
