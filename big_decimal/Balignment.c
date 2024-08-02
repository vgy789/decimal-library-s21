#include "big_decimal.h"

bool Bmul_by_ten(big_decimal *value) {
  uint8_t scale = Bget_scale(*value);
  bool is_err = Bs21_mul(*value, (big_decimal){{10, 0, 0, 0, 0, 0, 0}}, value);
  Bset_scale(value, ++scale);
  return is_err;
}

bool Bdiv_by_ten(big_decimal *value) {
  uint8_t scale = Bget_scale(*value);
  bool is_err = Bs21_div(*value, (big_decimal){{10, 0, 0, 0, 0, 0, 0}}, value);
  Bset_scale(value, ++scale);
  return is_err;
}

// TODO: test this
void Bcircumcision(big_decimal *value) {
  int8_t mant_size = Bget_scale(*value);

  while (mant_size > 0 && (*value).bits[0] % 10 == 0) {
    Bdiv_by_ten(value);
    mant_size--;
  }
  Bset_scale(value, mant_size);
}

void Balignment(big_decimal *value1, big_decimal *value2) {
  uint8_t mant_size1 = Bget_scale(*value1);
  uint8_t mant_size2 = Bget_scale(*value2);

  if (mant_size1 == mant_size2) {
    return;
  }
  if (mant_size1 > mant_size2) {
    Balignment(value2, value1);
    return;
  }

  while (mant_size1 < mant_size2) {
    if ((*value1).bits[2] > 0xFFFFFFF / 10) break;
    Bmul_by_ten(value1);
    mant_size1++;
  }

  Bset_scale(value1, mant_size1);
  while (mant_size1 < mant_size2) {
    if ((*value2).bits[0] % 10 != 0) break;
    Bdiv_by_ten(value2);
    mant_size2--;
  }

  Bset_scale(value2, mant_size2);
  while (mant_size1 < mant_size2) {
    Bdiv_by_ten(value2);
    mant_size2--;
  }
  Bset_scale(value2, mant_size2);
}
