#include "s21_decimal.h"

void magnitude_div10(s21_decimal *value) {
  big_decimal big = (big_decimal){{0}};
  Bdigits_div(big, (big_decimal){{10}}, &big, div);
  big_to_decimal(big, value);
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
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
    decimal_to_big(value_1, &big_1);
    decimal_to_big(value_2, &big_2);

    Balignment(&big_1, &big_2, 1);
    Bdigits_add(big_1, big_2, &big_result);
    Bset_scale(&big_result, Bget_scale(big_1));
    Bcircumcision(&big_result);
    err_code = big_to_decimal(big_result, result);
  }

  return err_code;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const bool sign = get_sign(value_2);
  // инвертируем знак и складываем
  set_sign(&value_2, !sign);
  const uint8_t err_code = s21_add(value_1, value_2, result);
  return err_code;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  big_decimal big_result = (big_decimal){{0}};
  *result = (s21_decimal){{0}};
  uint8_t err_code = 0;

  err_code = check_scale(value_1);
  if (err_code == 0) {
    err_code = check_scale(value_2);
  }

  if (err_code == 0) {
    decimal_to_big(value_1, &big_1);
    decimal_to_big(value_2, &big_2);
    const uint8_t scale_result = Bget_scale(big_1) + Bget_scale(big_2);

    (void)Bdigits_mul(big_1, big_2, &big_result);
    if (scale_result > 28) { /* слишком большой scale */
      if (Bget_sign(big_result) == plus) {
        return 1;
      } else {
        return 2;
      }
    }
    Bset_scale(&big_result, scale_result);
    Bcircumcision(&big_result);
    err_code = big_to_decimal(big_result, result);
  }

  return err_code;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
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
    decimal_to_big(value_1, &big_1);
    decimal_to_big(value_2, &big_2);
    const u_int8_t scale_1 = Bget_scale(big_1);
    const u_int8_t scale_2 = Bget_scale(big_2);

    uint8_t err_code = Bdigits_div(big_1, big_2, &big_result, div);
    if (err_code) {
      return err_code;
    }
    int8_t scale_result = scale_1 - scale_2 + Bget_scale(big_result);

    while (scale_result < 0) {
      Bdigits_mul10(&big_result);
      ++scale_result;
    }
    Bset_scale(&big_result, scale_result);
    Bcircumcision(&big_result);
    err_code = big_to_decimal(big_result, result);
  }

  return err_code;
}
