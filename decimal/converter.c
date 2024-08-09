#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

static size_t s21_strlen(const char *str) {
  const char *end = str;
  while (*str != '\0') str += 1;
  return end - str;
}

static void s21_strrev(char *str) {
  if (!str) {
    return;
  }
  int i = 0;
  int j = s21_strlen(str) - 1;

  while (i < j) {
    char c = str[i];
    str[i] = str[j];
    str[j] = c;
    i++;
    j--;
  }
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  uint8_t err_code = check_scale(src);
  if (err_code != 0) return err_code;
  if (src.bits[1] != 0 && src.bits[2] != 0) { /* is not overflow? */
    if (get_sign(src) == plus) {
      return 1;
    } else {
      return 2;
    }
  }

  uint8_t scale_src = get_scale(src);
  while (scale_src > 0) {
    divide10(src, &src);
    scale_src--;
  }
  *dst = src.bits[0];
  if (get_sign(src) == 1) {
    *dst = -*dst;
  }

  return 0;
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

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  *dst = 0;
  const bool sign = get_sign(src);
  if (sign == minus) {
    *dst = -(*dst);
  }

  s21_decimal buf = (s21_decimal){{0}};
  uint8_t scale = get_scale(src);

  char digits[33] = {'\0'};
  int digit = 0;

  uint8_t i = 0;
  do {
    if (scale == 0) {
      digits[i] = '.';
      i += 1;
    }
    modulus10(src, &buf);
    s21_from_decimal_to_int(buf, &digit);
    digits[i] = (char)(digit + '0');
    i += 1;
    scale -= 1;
    divide10(src, &src);
  } while (s21_is_not_equal(src, (s21_decimal){{0}}));

  s21_strrev(digits);
  *dst = strtof(digits, NULL);
  if (sign == minus) {
    *dst = -*dst;
  }

  return 0;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  const bool sign = signbit(src);
  if (isinf(src)) { /* inf */
    return 1;
    if (sign == minus) { /* -inf */
      return 2;
    }
  }
  if (isnan(src)) { /* nan or -nan */
    return 3;
  }

  char digits[38] = {'\0'};
  sprintf(digits, "%f", src);

  big_decimal Bdst = (big_decimal){{0}};
  src = fabs(src);
  bool has_point = false;
  uint8_t scale = 0;

  for (int i = 0; digits[i] != '\0'; ++i) {
    if (has_point) {
      scale += 1;
    }
    if ((digits[i] == '.')) {
      has_point = true;
      continue;
    }

    Bdigits_mul10(&Bdst);
    Bdigits_add(Bdst, (big_decimal){{digits[i] - '0'}}, &Bdst);
  }

  Bset_scale(&Bdst, scale);
  Bcircumcision(&Bdst);
  Bset_sign(&Bdst, sign);

  big_to_decimal(Bdst, dst);

  return 0;
}
