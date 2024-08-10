#include "s21_decimal.h"

typedef int (*calc_func)(big_decimal, big_decimal, big_decimal *);

uint8_t divide10(s21_decimal value, s21_decimal *result) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  const uint8_t err_code =
      Bdigits_division(big, (big_decimal){{10}}, &big, whole);
  big_to_decimal(big, result);
  return err_code;
}

uint8_t modulus10(s21_decimal value, s21_decimal *result) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  const uint8_t err_code =
      Bdigits_division(big, (big_decimal){{10}}, &big, reside);
  big_to_decimal(big, result);
  return err_code;
}

static int8_t calculate_scale(big_decimal value_1, big_decimal value_2,
                              big_decimal result, const calc_func calc) {
  int8_t scale_result = 0;
  if (calc == Bdigits_add) {
    scale_result = Bget_scale(value_1);
  } else if (calc == Bdigits_mul) {
    scale_result = Bget_scale(value_1) + Bget_scale(value_2);
  } else if (calc == Bdigits_div) {
    scale_result =
        Bget_scale(value_1) - Bget_scale(value_2) + Bget_scale(result);
  }
  return scale_result;
}

// calc ← Bdigits_add or Bdigits_mul
static int calculate(s21_decimal value_1, s21_decimal value_2,
                     s21_decimal *result, const calc_func calc) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  big_decimal big_result = (big_decimal){{0}};
  uint8_t err_code = 0;
  *result = (s21_decimal){{0}};

  err_code = check_scale(value_1);
  if (err_code == 0) {
    err_code = check_scale(value_2);
  }

  if (err_code == 0) {
    int8_t scale_result = 0;
    decimal_to_big(value_1, &big_1);
    decimal_to_big(value_2, &big_2);

    if (calc == Bdigits_add) {
      Balignment(&big_1, &big_2, 1);
    }
    err_code = calc(big_1, big_2, &big_result);
    scale_result = calculate_scale(big_1, big_2, big_result, calc);
    if (calc == Bdigits_div) {
      if (err_code) return err_code;
      while (scale_result < 0) {
        Bdigits_mul10(&big_result);
        ++scale_result;
      }
    }
    if (scale_result > 28) { /* слишком большой scale */
      return Bget_sign(big_result) + 1;
    }

    Bset_scale(&big_result, scale_result);
    Bcircumcision(&big_result);
    err_code = big_to_decimal(big_result, result);
  }

  return err_code;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const uint8_t err_code = calculate(value_1, value_2, result, Bdigits_add);
  return err_code;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_negate(value_2, &value_2);
  const uint8_t err_code = s21_add(value_1, value_2, result);
  return err_code;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const uint8_t err_code = calculate(value_1, value_2, result, Bdigits_mul);
  return err_code;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const uint8_t err_code = calculate(value_1, value_2, result, Bdigits_div);
  return err_code;
}
