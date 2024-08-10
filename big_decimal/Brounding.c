#include "big_decimal.h"

int Bs21_truncate(big_decimal value, big_decimal *result) {
  int count = Bget_scale(value);
  Bset_scale(&value, 0);
  for (int i = 0; i < count; i++) {
    // TODO: проверить! заменил div1 на reside
    Bdigits_div(value, (big_decimal){{10}}, &value, whole);
  }
  *result = value;
  return 0;
}

int Bs21_round(big_decimal value, big_decimal *result) {
  int count = Bget_scale(value);
  big_decimal temp = {{0}};
  Bs21_truncate(value, &temp);
  Bset_scale(&value, count - 1);
  big_decimal last_digit = {{0}};
  Bs21_truncate(value, &last_digit);

  if (Bmod(last_digit, (big_decimal){{10}}) >= 5) {
    if (Bget_sign(temp) == plus) {
      Bdigits_add(temp, (big_decimal){{1}}, result);
    } else {
      Bdigits_sub(temp, (big_decimal){{1}}, result);
    }
  } else {
    *result = temp;
  }
  return 0;
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
