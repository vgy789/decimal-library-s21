#include "s21_decimal.h"

#include "../bitwise_helper.h"

void set_sign(s21_decimal *dec, bool sign) {
  if (sign == plus) {
    dec->bits[3] &= ~(1 << 31);
  } else {
    dec->bits[3] |= (1 << 31);
  }
}

bool get_sign(s21_decimal dec) { return ((dec.bits[3] >> 31) & 1); }

void uint_binary(unsigned int value) {
  int bits = 31;

  // value = abs(value);
  while (value != 0) {
    int buf = value - (1 << bits);

    if (buf >= 0) {
      value = buf;
      printf("1");
    } else {
      printf("0");
    }
    --bits;
  }
  while (bits >= 0) {
    printf("0");
    --bits;
  }
}

void print_0b_decimal(s21_decimal value) {
  if (get_sign(value))
    printf("-");
  else
    printf("+");
  uint_binary(value.bits[2]);
  uint_binary(value.bits[1]);
  uint_binary(value.bits[0]);
}

// int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
//   return 0;
// }
