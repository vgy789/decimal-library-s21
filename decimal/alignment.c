#include "s21_decimal.h"

bool mul_by_ten(s21_decimal *value) {
  uint8_t scale = get_scale(*value);
  bool is_err = s21_mul(*value, (s21_decimal){{10, 0, 0, 0}}, value);
  set_scale(value, ++scale);
  return is_err;
}

bool div_by_ten(s21_decimal *value) {
  uint8_t scale = get_scale(*value);
  bool is_err = s21_div(*value, (s21_decimal){{10, 0, 0, 0}}, value);
  set_scale(value, ++scale);
  return is_err;
}

void alignment(s21_decimal *value1, s21_decimal *value2) {
  int mant_size1 = get_scale(*value1), mant_size2 = get_scale(*value2);
  if (mant_size1 == mant_size2) return;
  if (mant_size2 < mant_size1) {
    alignment(value2, value1);
    return;
  }
  while (mant_size1 < mant_size2) {
    if ((*value1).bits[2] > 0xFFFFFFF / 10) break;
    mul_by_ten(value1);
    mant_size1++;
  }

  set_scale(value1, mant_size1);
  while (mant_size1 < mant_size2) {
    if ((*value2).bits[0] % 10 != 0) break;
    div_by_ten(value2);
    mant_size2--;
  }

  set_scale(value2, mant_size2);
  while (mant_size1 < mant_size2) {
    div_by_ten(value2);
    mant_size2--;
  }
  set_scale(value2, mant_size2);
}
