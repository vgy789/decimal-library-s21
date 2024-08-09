#include "s21_decimal.h"

uint8_t check_scale(s21_decimal value) {
  if (get_scale(value) > 28) { /* max scale test */
    if (get_sign(value) == plus) {
      return 1;
    } else {
      return 2;
    }
  }
  return 0;  // correct scale
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
  uint8_t err_code = check_scale(value);

  if (err_code == 0) {
    decimal_to_big(value, &big);
    (void)Bdigits_mul(big, (big_decimal){{1, 0, 0, 0, 0, 0, minus_bit}}, &big);
  }
  err_code = big_to_decimal(big, result);
  return err_code;
}
