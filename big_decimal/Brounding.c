#include "big_decimal.h"

int Bs21_truncate(big_decimal value, big_decimal *result) {
  int count = Bget_scale(value);
  Bset_scale(&value, 0);
  for (int i = 0; i < count; i++) {
    Bs21_div2(value, (big_decimal){{10}}, &value);
  }
  *result = value;
  return 0;
}
