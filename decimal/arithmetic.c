#include "../s21_decimal.h"

typedef int (*calc_func)(big_decimal, big_decimal, big_decimal *);

err_t divide10(s21_decimal value, s21_decimal *result) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  const err_t err_code =
      Bdigits_division(big, (big_decimal){{10}}, &big, whole);
  big_to_decimal(big, result);
  return err_code;
}

err_t modulus10(s21_decimal value, s21_decimal *result) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  const err_t err_code =
      Bdigits_division(big, (big_decimal){{10}}, &big, reside);
  big_to_decimal(big, result);

  return err_code;
}

static scale_t calculate_scale(big_decimal value_1, big_decimal value_2,
                               big_decimal result, const calc_func calc) {
  scale_t scale_result = 0;
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
static err_t calculate(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result, const calc_func calc) {
  err_t err_code = 0;
  // scale должен быть в пределах (0, 28]
  err_code = check_scale(value_1);
  if (err_code) return err_code;
  err_code = check_scale(value_2);
  if (err_code) return err_code;

  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  big_decimal big_result = (big_decimal){{0}};
  *result = (s21_decimal){{0}};
  scale_t scale_result = 0;
  decimal_to_big(value_1, &big_1);
  decimal_to_big(value_2, &big_2);

  if (calc == Bdigits_add) {
    Balignment(&big_1, &big_2, 1);
  }
  err_code = calc(big_1, big_2, &big_result);
  scale_result = calculate_scale(big_1, big_2, big_result, calc);

  if (calc == Bdigits_div) {
    if (err_code) return err_code;
  }
  Bset_scale(&big_result, scale_result);
  Bnormalize(&big_result);

  if (Bget_scale(big_result) > MAX_SCALE) { /* слишком большой scale */
    err_code = Bget_sign(big_result) + 1;
  }

  if (err_code == 0) {
    Bfix_bank_overflow(&big_result);
    err_code = big_to_decimal(big_result, result);
  }

  return err_code;
}

err_t s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const err_t err_code = calculate(value_1, value_2, result, Bdigits_add);
  return err_code;
}

err_t s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_negate(value_2, &value_2);
  const err_t err_code = s21_add(value_1, value_2, result);
  return err_code;
}

err_t s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const err_t err_code = calculate(value_1, value_2, result, Bdigits_mul);
  return err_code;
}

err_t s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const err_t err_code = calculate(value_1, value_2, result, Bdigits_div);
  return err_code;
}
