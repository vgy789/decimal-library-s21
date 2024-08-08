#include "../big_decimal/big_decimal.h"
#include "s21_decimal.h"

int Bdec_to_int(big_decimal value) {
  s21_decimal dec_result = (s21_decimal){{0}};
  big_to_decimal(value, &dec_result);
  int result = 0;
  s21_from_decimal_to_int(dec_result, &result);
  return result;
}

uint8_t big_to_decimal(big_decimal value, s21_decimal *result) {
  *result = (s21_decimal){{0}};
  for (uint8_t i = 3; i < 6; ++i) {
    if (value.bits[i] != 0) { /* is overflow? */
      if (Bget_sign(value) == plus) {
        return 1;
      } else {
        return 2;
      }
    }
  }

  for (uint8_t i = 0; i < 3; ++i) {
    result->bits[i] = value.bits[i];
  }
  result->bits[3] = value.bits[6];
  return 0;
}

void decimal_to_big(s21_decimal value, big_decimal *result) {
  *result = (big_decimal){{0}};
  if (value.bits[0] == 0 && value.bits[1] == 0 &&
      value.bits[2] == 0) { /* is zero? */
    /* -0 to 0 */
    value.bits[3] = 0;
  }
  /* converter */
  for (uint8_t i = 0; i < 3; ++i) {
    result->bits[i] = value.bits[i];
  }
  result->bits[6] = value.bits[3];
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  uint8_t err_code = 0;
  *dst = 0;
  if (src.bits[1] == 0 && src.bits[2] == 0) { /* is not overflow? */
    uint8_t scale_src = get_scale(src);
    while (scale_src > 0) {
      magnitude_div10(&src);
      scale_src--;
    }
    *dst = src.bits[0];
    if (get_sign(src) == 1) {
      *dst = -*dst;
    }
  } else {
    err_code = 1;
  }

  return err_code;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  *dst = (s21_decimal){{0}};
  if (src < 0) { /* is negative? */
    src = -src;
    set_sign(dst, minus);
  }
  dst->bits[0] = src;

  return 0;
}
