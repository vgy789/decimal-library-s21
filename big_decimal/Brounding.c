#include "../test/debug_helper.h"
#include "big_decimal.h"

void Bs21_truncate(big_decimal value, big_decimal *result) {
  scale_t scale = Bget_scale(value);
  Bset_scale(&value, 0);
  for (uint16_t i = 0; i < scale; i++) {
    Bdigits_division(value, (big_decimal){{10}}, &value, whole);
  }
  *result = value;
}

void Bs21_round(big_decimal value, big_decimal *result) {
  scale_t scale = Bget_scale(value);
  if (scale > 0) {
    big_decimal temp = {{0}};
    Bs21_truncate(value, &temp);

    Bset_scale(&value, scale - 1);
    big_decimal last_digit = {{0}};
    Bs21_truncate(value, &last_digit);

    if (Bmod(last_digit, (big_decimal){{10}}) >= 5) {
      Bdigits_add(temp, (big_decimal){{1}}, result);
    } else {
      *result = temp;
    }
  } else {
    *result = value;
  }
}

void Bs21_floor(big_decimal value, big_decimal *result) {
  int scale = Bget_scale(value);
  if (scale > 0) {
    big_decimal temp = {{0}};
    Bs21_truncate(value, &temp);
    Bset_scale(&value, scale - 1);
    if (Bget_sign(value) == minus) {
      Bdigits_add(temp, (big_decimal){{1}}, result);
    } else {
      *result = temp;
    }
  } else {
    *result = value;
  }
}

void Bbank_round(big_decimal value, big_decimal *result) {
  const scale_t scale = Bget_scale(value);
  big_decimal temp = {{0}};
  big_decimal last_digit = {{0}};

  if (scale > 0) {
    Bs21_truncate(value, &temp);
    Bset_scale(&value, scale - 1);
    Bs21_truncate(value, &last_digit);

    const int mod = Bmod(last_digit, (big_decimal){{10}});
    if ((mod == 5 && Bget_bit(temp, 0) == 1) || mod > 5) {
      Bdigits_add(temp, (big_decimal){{1}}, result);
    } else {
      *result = temp;
    }
  }
}
