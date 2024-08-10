#include "s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  uint8_t err_code = check_scale(value);
  if (err_code == 0) {
    const bool sign = get_sign(value);
    big_decimal big = (big_decimal){{0}};
    decimal_to_big(value, &big);
    Bs21_truncate(big, &big);
    err_code = big_to_decimal(big, result);
    set_sign(result, sign);
  }
  return err_code;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  uint8_t err_code = check_scale(value);
  if (err_code == 0) {
    big_decimal big = (big_decimal){{0}};
    decimal_to_big(value, &big);
    Bs21_round(big, &big);
    err_code = big_to_decimal(big, result);
  }
  return err_code;
}

int Bs21_floor(big_decimal value, big_decimal *result) {
  int count = Bget_scale(value);
  big_decimal temp = {{0}};
  Bs21_truncate(value, &temp);
  Bset_scale(&value, count - 1);
  big_decimal last_digit = {{0}};
  Bs21_truncate(value, &last_digit);
  if (Bget_sign(temp) == minus) {
      Bdigits_sub(temp, (big_decimal){{1}}, result);
  } else {
  *result = temp;
  }
  return 0;
}
