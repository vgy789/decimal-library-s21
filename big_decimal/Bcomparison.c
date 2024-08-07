#include "big_decimal.h"

// сравнивает мантиссы без учёта знака и scale
static int Bcomparison_mantiss(big_decimal value_1, big_decimal value_2) {
  int16_t bit_pos = BDIGITS_BIT_COUNT - 1;
  bool pos_a, pos_b;
  u_int8_t result = 0;

  while (bit_pos >= 0) {
    pos_a = Bget_bit(value_1, bit_pos);
    pos_b = Bget_bit(value_2, bit_pos);
    if (pos_a > pos_b) {
      result = 1;
      break;
    }
    if (pos_a < pos_b) {
      result = 2;
      break;
    }
    if (pos_a == pos_b) bit_pos--;
  }

  return result;  // 0 → a==b, 1 → a > b, 2 → a < b
}

// сравнивает числа с учетом знака и scale
static int Bcomparison(big_decimal a, big_decimal b) {
  uint8_t result = 0;
  bool sign_a = Bget_sign(a);
  bool sign_b = Bget_sign(b);

  if (sign_a > sign_b) {
    result = 2;  // а - отрицательное число, b - положительное
  } else if (sign_a < sign_b) {
    result = 1;  // b - отрицательное число, a - положительное
  } else { /* a == b */
    uint8_t scale_a = Bget_scale(a);
    uint8_t scale_b = Bget_scale(b);
    if (scale_a == scale_b)
      result = Bcomparison_mantiss(a, b);  // если экспоненты равны
    else {
      Balignment(&a, &b);
      result = Bcomparison_mantiss(a, b);
    }
    if (scale_a == 1) result = -result;
  }

  return result;  // 0 → a==b, 1 → a > b, 2 → a < b
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
  return !Bdigits_lt(value_1, value_2);
}

int Bdigits_gt(big_decimal value_1, big_decimal value_2) {
  return Bcomparison_mantiss(value_1, value_2) == 1;
}

int Bdigits_le(big_decimal value_1, big_decimal value_2) {
  return !Bdigits_gt(value_1, value_2);
}

int Bdigits_lt(big_decimal value_1, big_decimal value_2) {
  return Bcomparison_mantiss(value_1, value_2) == 2;
}

int Bs21_is_equal(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison(value_1, value_2);
  return !result;
}

int Bs21_is_not_equal(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison(value_1, value_2);
  return result;
}

int Bs21_is_greater(big_decimal value_1, big_decimal value_2) {
  return Bcomparison(value_1, value_2) == 1;
}

int Bs21_is_greater_or_equal(big_decimal value_1, big_decimal value_2) {
  return !Bs21_is_less(value_1, value_2);
}

int Bs21_is_less(big_decimal value_1, big_decimal value_2) {
  return Bcomparison(value_1, value_2) == 2;
}

int Bs21_is_less_or_equal(big_decimal value_1, big_decimal value_2) {
  return !Bs21_is_greater(value_1, value_2);
}
