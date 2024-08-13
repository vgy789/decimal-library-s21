#include "../s21_decimal.h"

typedef void (*roundness)(big_decimal, big_decimal *);

err_t rounding(s21_decimal value, s21_decimal *result, roundness roundness) {
  err_t err_code = check_scale(value);
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

err_t s21_truncate(s21_decimal value, s21_decimal *result) {
  return rounding(value, result, Bs21_truncate);
}

err_t s21_round(s21_decimal value, s21_decimal *result) {
  return rounding(value, result, Bs21_round);
}

err_t s21_floor(s21_decimal value, s21_decimal *result) {
  return rounding(value, result, Bs21_floor);
}

err_t bank_round(s21_decimal value, s21_decimal *result) {
  return rounding(value, result, Bbank_round);
}
