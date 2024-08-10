#include "s21_decimal.h"

typedef int (*roundness)(big_decimal, big_decimal *);

int rounding(s21_decimal value, s21_decimal *result, roundness roundness) {
  uint8_t err_code = check_scale(value);
  if (err_code == 0) {
    const bool sign = get_sign(value);
    big_decimal big = (big_decimal){{0}};
    decimal_to_big(value, &big);
    roundness(big, &big);
    err_code = big_to_decimal(big, result);
    set_sign(result, sign);
  }
  return err_code;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  return rounding(value, result, Bs21_truncate);
}

int s21_round(s21_decimal value, s21_decimal *result) {
  return rounding(value, result, Bs21_round);
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  return rounding(value, result, Bs21_floor);
}
