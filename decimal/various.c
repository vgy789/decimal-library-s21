#include "s21_decimal.h"

bool is_correct_scale(s21_decimal value) {
  return get_scale(value) <= 28; /* max scale test */
}

uint8_t get_scale(s21_decimal value) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  return Bget_scale(big);
}

bool get_sign(s21_decimal value) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  return Bget_sign(big);
}

void set_sign(s21_decimal *value, bool sign) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bset_sign(&big, sign);
  big_to_decimal(big, value);
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  enum { minus_bit = 0x80000000 };
  big_decimal big = {0};

  bool err_code = 0;
  if (!is_correct_scale(value)) {
    err_code = 4;
  }
  if (err_code == 0) {
    decimal_to_big(value, &big);
    (void)Bs21_mul(big, (big_decimal){{1, 0, 0, 0, 0, 0, minus_bit}}, &big);
  }
  err_code = big_to_decimal(big, result);
  return err_code;
}
