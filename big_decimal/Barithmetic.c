#include "big_decimal.h"

static bool Badd_word(uint32_t *result, uint32_t value_1, uint32_t value_2,
                      bool transfer) {
  bool overflow = 0;
  overflow += __builtin_uadd_overflow(value_1, value_2, result);
  overflow += __builtin_uadd_overflow(*result, transfer, result);
  return overflow;
}

void Bcompliment2(big_decimal value, big_decimal *result) {
  for (uint8_t i = 0; i < 6; ++i) {
    value.bits[i] = ~value.bits[i];
  }
  Bincrement(value, result);
}

bool Bmagnitude_div10(big_decimal *value) {
  // const uint8_t scale = Bget_scale(*value);
  const uint8_t err_code =
      Bs21_div(*value, (big_decimal){{10, 0, 0, 0, 0, 0}}, value);
  // Bset_scale(value, scale - 1);
  return err_code;
}

bool Bmagnitude_mul10(big_decimal *value) {
  // const uint8_t scale = Bget_scale(*value);
  const uint8_t err_code =
      Bs21_mul(*value, (big_decimal){{10, 0, 0, 0, 0, 0}}, value);
  // Bset_scale(value, scale + 1);
  return err_code;
}

int Bdecriment(big_decimal value, big_decimal *result) {
  const big_decimal one = (big_decimal){{1, 0, 0, 0, 0, 0, 0}};
  Bmagnitude_sub(value, one, result);
  return 0;
}

int Bincrement(big_decimal value, big_decimal *result) {
  const big_decimal one = (big_decimal){{1, 0, 0, 0, 0, 0, 0}};
  Bmagnitude_add(value, one, result);
  return 0;
}

int Bmagnitude_add(big_decimal value_1, big_decimal value_2,
                   big_decimal *result) {
  *result = (big_decimal){{0}};
  const bool sign_1_orig = Bget_sign(value_1);
  const bool sign_2_orig = Bget_sign(value_2);

  /* чтобы узнать знак результата необходимо сравнить биты не учитывая знак. */
  const bool result_sign =
      (sign_1_orig == minus && sign_2_orig == minus) ||
      (sign_1_orig == minus && Bmagnitude_gt(value_1, value_2)) ||
      (sign_2_orig == minus && Bmagnitude_lt(value_1, value_2));

  // при необходимости переводим в дополнительный код.
  if (sign_1_orig == minus) {
    Bset_sign(&value_1, plus);  // complement2 использует сложение. Чтобы не
                                // уйти в рекурсию ставлю знак в +.
    Bcompliment2(value_1, &value_1);
  }
  if (sign_2_orig == minus) {
    Bset_sign(&value_2, plus);
    Bcompliment2(value_2, &value_2);
  }

  /* сложение */
  bool transfer = 0;
  for (uint8_t i = 0; i < 6; ++i) {
    transfer = Badd_word((uint32_t *)&result->bits[i], value_1.bits[i],
                         value_2.bits[i], transfer);
  }

  uint8_t err_code = 0;
  if (result_sign == minus) {
    Bcompliment2(*result, result);
  }

  if (result->bits[3] != 0) {
    err_code = 1;
    if (result_sign == minus) {
      err_code = 2;
    }
  }
  Bset_result_sign(result, result_sign);

  return err_code;
}

int Bmagnitude_sub(big_decimal value_1, big_decimal value_2,
                   big_decimal *result) {
  const bool sign = Bget_sign(value_2);
  // инвертируем знак и складываем
  Bset_sign(&value_2, !sign);
  uint8_t err_code = Bmagnitude_add(value_1, value_2, result);

  return err_code;
}

int Bs21_mul(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  const bool sign_1_orig = Bget_sign(value_1);
  const bool sign_2_orig = Bget_sign(value_2);

  const bool result_sign = (sign_1_orig == minus || sign_2_orig == minus) &&
                           !(sign_1_orig == minus && sign_2_orig == minus);
  {
    // оптимизация умножения :-)
    if (Bmagnitude_gt(value_2, value_1)) {
      Bswap(&value_1, &value_2);
    }
    // TODO: этот код по-сути меняет знаки val_1 и val_2 местами. Если оставить
    // знаки на своих местах, то тесткейс зависнет. ПОЧЕМУ
    Bset_sign(&value_1, sign_1_orig);
    Bset_sign(&value_2, sign_2_orig);
  }

  if (sign_1_orig == minus) Bset_sign(&value_1, plus);
  if (sign_2_orig == minus) Bset_sign(&value_2, plus);

  uint8_t err_code = 0;
  *result = (big_decimal){0};
  // умножение через сложение
  while (Bmagnitude_ne(value_2, (big_decimal){{0}})) {
    Bdecriment(value_2, &value_2);
    err_code = Bmagnitude_add(value_1, *result, result);
    if (err_code) {
      err_code = 1;
      if (result_sign == minus) err_code = 2;
      break;
    }
  }
  Bset_result_sign(result, result_sign);

  return err_code;
}

// https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
int Bs21_div(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  if (Bmagnitude_eq(value_2, (big_decimal){{0}})) /* если делим на 0 */
    return 3;

  uint8_t err_code = 0;
  const bool sign_1 = Bget_sign(value_1);
  const bool sign_2 = Bget_sign(value_2);
  const bool result_sign = (sign_1 == minus || sign_2 == minus) &&
                           !(sign_1 == minus && sign_2 == minus);
  Bset_sign(&value_1, plus);
  Bset_sign(&value_2, plus);

  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R = (big_decimal){{0}};  // остаток remainder
  while (1) {
    for (u_int8_t i = 0; i < 6; ++i) { /* сбрасывает значение Q */
      Q.bits[i] = 0;
    }
    R = (big_decimal){{0}};

    for (int i = BMAGNITUDE_BIT_COUNT - 1; i >= 0; i--) {
      Bleft_shift(&R);
      Bset_bit(&R, 0, Bget_bit(value_1, i));
      if (Bmagnitude_ge(R, value_2)) {
        (void)Bmagnitude_sub(R, value_2, &R);
        Bset_bit(&Q, i, 1);
      }
    }

    *result = Q;

    // домножаем на 10 и снова делим, пока остаток не станет 0.
    if (Bmagnitude_ne(R, (big_decimal){{0}})) {
      const bool overflow = Bmagnitude_mul10(&value_1);
      Bset_scale(&Q, Bget_scale(Q) + 1);
      if (overflow) { /* слишком много цифр после запятой */
        *result = Q;
        if (result_sign == plus) {
          err_code = 1;
        } else {
          err_code = 2;
        }
        break;
      }
    } else {
      break;
    }
  }
  Bset_result_sign(result, result_sign);

  return err_code;
}

int Bs21_div2(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  if (Bmagnitude_eq(value_2, (big_decimal){{0}})) /* если делим на 0 */
    return 2;

  const bool sign_1 = Bget_sign(value_1);
  const bool sign_2 = Bget_sign(value_2);
  const bool result_sign = (sign_1 == minus || sign_2 == minus) &&
                           !(sign_1 == minus && sign_2 == minus);
  Bset_sign(&value_1, plus);
  Bset_sign(&value_2, plus);

  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R = (big_decimal){{0}};  // остаток remainder

  for (u_int8_t i = 0; i < 6; ++i) { /* сбрасывает значение Q */
    Q.bits[i] = 0;
  }
  R = (big_decimal){{0}};

  for (int i = BMAGNITUDE_BIT_COUNT - 1; i >= 0; i--) {
    Bleft_shift(&R);
    Bset_bit(&R, 0, Bget_bit(value_1, i));
    if (Bmagnitude_ge(R, value_2)) {
      (void)Bmagnitude_sub(R, value_2, &R);
      Bset_bit(&Q, i, 1);
    }
  }

  *result = Q;
  Bset_result_sign(result, result_sign);

  return 0;
}
