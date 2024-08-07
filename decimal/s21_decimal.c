#include "s21_decimal.h"

uint8_t big_to_decimal(big_decimal value, s21_decimal *result) {
  for (uint8_t i = 3; i < 6; ++i) {
    if (value.bits[i] != 0) { /* тест на переполнение */
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
  for (uint8_t i = 0; i < 3; ++i) {
    result->bits[i] = value.bits[i];
  }
  result->bits[6] = value.bits[3];
}

uint8_t get_scale(s21_decimal value) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  return Bget_scale(big);
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  big_decimal big_result = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);

  Balignment(&big_1, &big_2);

  Bmantiss_add(big_1, big_2, &big_result);

  Bset_scale(&big_result, Bget_scale(big_1));
  Bcircumcision(&big_result);

  return big_to_decimal(big_result, result);
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const bool sign = get_sign(value_2);
  // инвертируем знак и складываем
  set_sign(&value_2, !sign);
  return s21_add(value_1, value_2, result);
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  big_decimal big_result = (big_decimal){{0}};

  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);

  const uint8_t scale_result = Bget_scale(big_1) + Bget_scale(big_2);

  Bs21_mul(big_1, big_2, &big_result);
  if (scale_result > 28) { /* слишком большой scale */
    if (Bget_sign(big_result) == plus) {
      return 1;
    } else {
      return 2;
    }
  }
  Bset_scale(&big_result, scale_result);
  Bcircumcision(&big_result);

  return big_to_decimal(big_result, result);
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  *result = (s21_decimal){{0}};
  big_decimal big_result = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  const u_int8_t scale_1 = Bget_scale(big_1);
  const u_int8_t scale_2 = Bget_scale(big_2);

  if (Bs21_div(big_1, big_2, &big_result) == 2) {
    return 3; /* деление на 0 */
  }
  int8_t scale_result = scale_1 - scale_2 + Bget_scale(big_result);

  while (scale_result < 0) {
    Bmantiss_mul10(&big_result);
    ++scale_result;
  }
  Bset_scale(&big_result, scale_result);
  Bcircumcision(&big_result);

  return big_to_decimal(big_result, result);
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_equal(big_1, big_2);
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_not_equal(big_1, big_2);
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_greater(big_1, big_2);
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_greater_or_equal(big_1, big_2);
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_less(big_1, big_2);
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_less_or_equal(big_1, big_2);
}

static void mantiss_div10(s21_decimal *value) {
  big_decimal big = (big_decimal){{0}};
  Bs21_div(big, (big_decimal){{10}}, &big);
  big_to_decimal(big, value);
}

// TODO: проверить
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int result = 0;
  if (src.bits[1] == 0 && src.bits[2] == 0) {
    uint8_t scale_src = get_scale(src);
    while (scale_src > 0) {
      mantiss_div10(&src);
      scale_src--;
    }
    *dst = src.bits[0];
    if (get_sign(src) == 1) {
      *dst = -*dst;
    }
  } else {
    result = 1;
  }

  return result;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  if (src < 0) {
    src = -src;
    set_sign(dst, minus);
  }
  dst->bits[0] = src;

  return 0;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  enum { minus_bit = 0x80000000 };
  big_decimal big;

  decimal_to_big(value, &big);
  (void)Bs21_mul(big, (big_decimal){{1, 0, 0, 0, 0, 0, minus_bit}}, &big);
  const bool err_code = big_to_decimal(big, result);
  return err_code;
}
