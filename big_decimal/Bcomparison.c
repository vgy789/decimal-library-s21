#include "big_decimal.h"

enum { eq, gt, lt };

// сравнивает мантиссы без учёта знака и scale
static int Bcomparison_mantiss(big_decimal value_1, big_decimal value_2) {
  int16_t bit_pos = BDIGITS_BIT_COUNT - 1;
  bool pos_a, pos_b;
  u_int8_t result = eq;

  while (bit_pos >= 0) {
    pos_a = Bget_bit(value_1, bit_pos);
    pos_b = Bget_bit(value_2, bit_pos);
    if (pos_a > pos_b) {
      result = gt;
      break;
    }
    if (pos_a < pos_b) {
      result = lt;
      break;
    }
    if (pos_a == pos_b) bit_pos--;
  }

  return result;
}

// сравнивает числа с учетом знака и scale
static int Bcomparison(big_decimal a, big_decimal b) {
  uint8_t result = 0;
  bool sign_a = Bget_sign(a);
  bool sign_b = Bget_sign(b);

  if (sign_a > sign_b) {
    result = lt;  // а - отрицательное число, b - положительное
  } else if (sign_a < sign_b) {
    result = gt;  // b - отрицательное число, a - положительное
  } else { /* a == b */
    uint8_t scale_a = Bget_scale(a);
    uint8_t scale_b = Bget_scale(b);
    if (scale_a == scale_b)
      result = Bcomparison_mantiss(a, b);  // если экспоненты равны
    else {
      Balignment(&a, &b, 0);
      result = Bcomparison_mantiss(a, b);
    }
    if (scale_a == 1) result = -result;
  }

  return result;
}

int Bdigits_eq(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison_mantiss(value_1, value_2);
  return !result;
}

int Bdigits_ne(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison_mantiss(value_1, value_2);
  return result;
}

int Bdigits_ge(big_decimal value_1, big_decimal value_2) {
  const bool result = Bdigits_lt(value_1, value_2);
  return !result;
}

int Bdigits_gt(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison_mantiss(value_1, value_2) == gt;
  return result;
}

int Bdigits_le(big_decimal value_1, big_decimal value_2) {
  const bool result = !Bdigits_gt(value_1, value_2);
  return result;
}

int Bdigits_lt(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison_mantiss(value_1, value_2) == lt;
  return result;
}

int Bs21_is_equal(big_decimal value_1, big_decimal value_2) {
  const bool result = (bool)Bcomparison(value_1, value_2);
  return !result;
}

int Bs21_is_not_equal(big_decimal value_1, big_decimal value_2) {
  const bool result = (bool)Bcomparison(value_1, value_2);
  return result;
}

int Bs21_is_greater(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison(value_1, value_2) == gt;
  return result;
}

int Bs21_is_greater_or_equal(big_decimal value_1, big_decimal value_2) {
  const bool result = Bs21_is_less(value_1, value_2);
  return !result;
}

int Bs21_is_less(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison(value_1, value_2) == lt;
  return result;
}

int Bs21_is_less_or_equal(big_decimal value_1, big_decimal value_2) {
  const bool result = Bs21_is_greater(value_1, value_2);
  return !result;
}
