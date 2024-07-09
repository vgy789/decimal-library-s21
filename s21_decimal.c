#include "s21_decimal.h"

void set_sign(s21_decimal *dec, bool sign) {
  if (sign == 0) {
    dec->bits[3] &= ~(1 << 30);
  } else {
    dec->bits[3] |= (1 << 30);
  }
}

bool get_sign(s21_decimal dec){
  return ((dec.bits[3] >> 30) & 1);
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  bool overflow = __builtin_sadd_overflow(value_1.bits[0], value_2.bits[0], &result->bits[0]);
  
  return 0;
}

void print_decimal_bin(s21_decimal value) {
  // 2000000000
  // 0x77359400
  // printf("%#x %#x", value.bits[1], value.bits[0]);
}
