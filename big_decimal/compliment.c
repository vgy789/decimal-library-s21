#include "big_decimal.h"

void Bcompliment2(big_decimal value, big_decimal *result) {
  value.bits[0] = ~value.bits[0];
  value.bits[1] = ~value.bits[1];
  value.bits[2] = ~value.bits[2];
  value.bits[3] = ~value.bits[3];
  value.bits[4] = ~value.bits[4];
  value.bits[5] = ~value.bits[5];
  Bs21_inc(value, result);
}
