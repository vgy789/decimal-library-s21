#include "big_decimal.h"

void Bcompliment2(big_decimal value, big_decimal *result) {
  for (uint8_t i = 0; i < 6; ++i) {
    value.bits[i] = ~value.bits[i];
  }
  Bs21_inc(value, result);
}
