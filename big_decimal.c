#include "big_decimal.h"

void set_sign_big(big_decimal *dec, bool sign) {
  if (sign == 0) {
    dec->bits[2] &= ~(1 << 30);
  } else {
    dec->bits[2] |= (1 << 30);
  }
}

bool get_sign_big(big_decimal dec){
  return ((dec.bits[2] >> 30) & 1);
}

int s21_add_big(big_decimal value_1, big_decimal value_2, big_decimal *result) {

  return 0;
}

void print_0b_big(big_decimal value) {
  if (get_sign_big(value)) 
    printf("-");
  else 
    printf("+");
  ulong_binary(value.bits[1]);
  printf(" ");
  ulong_binary(value.bits[0]);
}

void ulong_binary(unsigned long value) {
  int bits = 63;

  value = labs(value);
  while (value != 0) {
    long buf = value - (1l << bits);

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
