#include "s21_decimal.h"

void decimal_to_big(s21_decimal value, big_decimal *result) {
  for (uint8_t i = 0; i < 3; ++i) {
    result->bits[i] = value.bits[i];
  }
  result->bits[6] = value.bits[3];
}

uint8_t big_to_decimal(big_decimal value, s21_decimal *result) {
  Bcircumcision(&value);
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

int s21_negate(s21_decimal value, s21_decimal *result) {
  enum { minus_bit = 0x80000000 };
  big_decimal big;

  decimal_to_big(value, &big);
  (void)Bs21_mul(big, (big_decimal){{1, 0, 0, 0, 0, 0, minus_bit}}, &big);
  const bool err_code = big_to_decimal(big, result);
  return err_code;
}

static void set_result_sign(s21_decimal *value, bool sign) {
  if (mantiss_eq(*value, (s21_decimal){{0}})) { /* -0 to +0 */
    set_sign(value, plus);
  } else {
    set_sign(value, sign);
  }
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  big_decimal big_result = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  // debug
  // {
  //   printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  //   Bdec_2bin(big_1, 1, 1);
  //   printf("\n");
  //   Bdec_2bin(big_2, 1, 1);
  //   printf("\n");
  //   printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  // }
  Bs21_add(big_1, big_2, &big_result);
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
  Bs21_mul(big_1, big_2, &big_result);
  return big_to_decimal(big_result, result);
}

// https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (mantiss_eq(value_2, (s21_decimal){{0}})) /* если делим на 0 */
    return 2;

  const bool sign_1 = get_sign(value_1);
  const bool sign_2 = get_sign(value_2);
  const bool result_sign = (sign_1 == minus || sign_2 == minus) &&
                           !(sign_1 == minus && sign_2 == minus);
  set_sign(&value_1, plus);
  set_sign(&value_2, plus);

  s21_decimal Q = (s21_decimal){{0}};  // частное quotient
  s21_decimal R = (s21_decimal){{0}};  // остаток remainder
  while (1) {
    Q.bits[0] = 0;
    Q.bits[1] = 0;
    Q.bits[2] = 0;
    R = (s21_decimal){{0}};

    for (int i = MANTISS_BIT_COUNT - 1; i >= 0; i--) {
      left_shift(&R);
      set_bit(&R, 0, get_bit(value_1, i));
      if (mantiss_ge(R, value_2)) {
        (void)s21_sub(R, value_2, &R);
        set_bit(&Q, i, 1);
      }
    }

    *result = Q;

    // домножаем на 10 и снова делим, пока остаток не станет 0.
    if (mantiss_ne(R, (s21_decimal){{0}})) {
      const bool overflow = mul_by_ten(&value_1);
      set_scale(&Q, get_scale(Q) + 1);
      if (overflow) { /* слишком много цифр после запятой */
        break;
      }
    } else {
      break;
    }
  }
  set_result_sign(result, result_sign);

  return 0;
}

bool mantiss_eq(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bmantiss_eq(big_1, big_2);
}

bool mantiss_ne(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bmantiss_ne(big_1, big_2);
}

bool mantiss_lt(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bmantiss_lt(big_1, big_2);
}

bool mantiss_gt(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bmantiss_gt(big_1, big_2);
}

bool mantiss_le(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bmantiss_le(big_1, big_2);
}

bool mantiss_ge(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bmantiss_ge(big_1, big_2);
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

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_less(big_1, big_2);
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_greater(big_1, big_2);
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_less_or_equal(big_1, big_2);
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);
  return Bs21_is_greater_or_equal(big_1, big_2);
}

int s21_inc(s21_decimal value, s21_decimal *result) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  (void)Bs21_inc(big, &big);
  return big_to_decimal(big, result);
}

int s21_dec(s21_decimal value, s21_decimal *result) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  (void)Bs21_dec(big, &big);
  return big_to_decimal(big, result);
}
