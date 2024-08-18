#include "../s21_decimal.h"
#include "../test/debug_helper.h"
#include "big_decimal.h"

static bool Badd_word(uint32_t *result, uint32_t value_1, uint32_t value_2,
                      bool transfer) {
  bool overflow = 0;
  overflow += __builtin_uadd_overflow(value_1, value_2, result);
  overflow += __builtin_uadd_overflow(*result, transfer, result);
  return overflow;
}

void Bfix_bank_overflow(big_decimal *value) {
  const bool sign = Bget_sign(*value);  // сохранить знак, т.к. в процессе
                                        // вычислений он станет невалидным

  scale_t scale = Bget_scale(*value);
  const scale_t scale_orig = scale;
  big_decimal Blast_digit = (big_decimal){{0}};
  int8_t last_digit = -1;
  uint8_t counter = 0;
  bool prev_nezero = false;

  while (((value->bits[3] != 0 || value->bits[4] != 0 || value->bits[5] != 0) &&
          scale > 0) ||
         scale > MAX_SCALE) {
    if (last_digit > 0) {
      prev_nezero = true;
    }

    Bmodulus10(*value, &Blast_digit);
    last_digit = Blast_digit.bits[0] % 10;
    Bdivide10(*value, value);
    scale -= 1;
    ++counter;
  }

  if (last_digit > -1) {
    if (last_digit == 5 && prev_nezero) {
      last_digit += 1;
    }
    Bdigits_mul10(value);
    Bdigits_add(*value, (big_decimal){{last_digit}}, value);
    Bset_scale(value, 1);
    Bbank_round(*value, value);
    Bset_scale(value, scale_orig - counter);
  }
  Bset_sign(value, sign);
}

void Bcompliment2(big_decimal value, big_decimal *result) {
  for (uint8_t i = 0; i < 6; ++i) {
    value.bits[i] = ~value.bits[i];
  }
  Bincrement(value, result);
}

err_t Bdigits_div10(big_decimal *value) {
  const err_t err_code = Bdigits_division(
      *value, (big_decimal){{10, 0, 0, 0, 0, 0}}, value, divide);
  return err_code;
}

err_t Bdigits_mul10(big_decimal *value) {
  const err_t err_code =
      Bdigits_mul(*value, (big_decimal){{10, 0, 0, 0, 0, 0}}, value);
  return err_code;
}

err_t Bdivide10(big_decimal value, big_decimal *result) {
  const err_t err_code =
      Bdigits_division(value, (big_decimal){{10}}, result, whole);
  return err_code;
}

err_t Bmodulus10(big_decimal value, big_decimal *result) {
  const err_t err_code =
      Bdigits_division(value, (big_decimal){{10}}, result, reside);
  return err_code;
}

err_t Bdecriment(big_decimal value, big_decimal *result) {
  const big_decimal one = (big_decimal){{1, 0, 0, 0, 0, 0, 0}};
  const err_t err_code = Bdigits_sub(value, one, result);
  return err_code;
}

err_t Bincrement(big_decimal value, big_decimal *result) {
  const big_decimal one = (big_decimal){{1, 0, 0, 0, 0, 0, 0}};
  return Bdigits_add(value, one, result);
}

err_t Bdigits_add(big_decimal value_1, big_decimal value_2,
                  big_decimal *result) {
  const big_decimal value_1_orig = value_1;
  const big_decimal value_2_orig = value_2;
  *result = (big_decimal){{0}};
  const bool sign_1_orig = Bget_sign(value_1);
  const bool sign_2_orig = Bget_sign(value_2);

  /* чтобы узнать знак результата необходимо сравнить биты не учитывая знак. */
  const bool result_sign =
      (sign_1_orig == minus && sign_2_orig == minus) ||
      (sign_1_orig == minus && Bdigits_gt(value_1, value_2)) ||
      (sign_2_orig == minus && Bdigits_lt(value_1, value_2));

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

  if (result_sign == minus) {
    Bcompliment2(*result, result);
  }

  err_t err_code = 0;

  if (sign_1_orig == sign_2_orig) { /* проверка переполнения big_decimal */
    // если результат суммы положительных/отрицательных меньше/больше, значит
    // ошибка переполнения.
    if (Bdigits_gt(value_1_orig, *result) ||
        Bdigits_gt(value_2_orig, *result)) {
      err_code = 1;
      if (result_sign) {
        err_code = 2;
      }
    }
  }
  Bset_sign(result, result_sign);

  return err_code;
}

err_t Bdigits_sub(big_decimal value_1, big_decimal value_2,
                  big_decimal *result) {
  Bset_sign(&value_2, !Bget_sign(value_2));
  const err_t err_code = Bdigits_add(value_1, value_2, result);
  return err_code;
}

err_t Bdigits_mul(big_decimal value_1, big_decimal value_2,
                  big_decimal *result) {
  *result = (big_decimal){{0}};
  const bool sign_1_orig = Bget_sign(value_1);
  const bool sign_2_orig = Bget_sign(value_2);
  const bool result_sign = (sign_1_orig == minus || sign_2_orig == minus) &&
                           !(sign_1_orig == minus && sign_2_orig == minus);

  bool overflow = 0;
  while (Bdigits_ne(value_2, (big_decimal){{0}})) {
    if (Bget_bit(value_2, 0) == 1) {
      Bdigits_add(*result, value_1, result);
    }
    Bright_shift(&value_2);
    overflow = Bleft_shift(&value_1);
    if (overflow) {
      break;
    }
  }
  Bset_sign(result, result_sign);

  err_t err_code = 0;
  if (overflow) {
    if (result_sign == plus) {
      err_code = 1;
    } else {
      err_code = 2;
    }
  }
  return err_code;
}

err_t Bdigits_div(big_decimal value_1, big_decimal value_2,
                  big_decimal *result) {
  const err_t err_code = Bdigits_division(value_1, value_2, result, divide);
  return err_code;
}

err_t NEWBdigits_div(big_decimal value_1, big_decimal value_2,
                     big_decimal *result) {
  const err_t err_code = NEWBdigits_division(value_1, value_2, result, divide);
  return err_code;
}

err_t NEWBdigits_division(big_decimal value_1, big_decimal value_2,
                          big_decimal *result, uint8_t mode) {
  if (Bdigits_eq(value_2, (big_decimal){{0}})) /* если делим на 0 */
    return 3;

  err_t err_code = 0;
  const bool sign_1 = Bget_sign(value_1);
  const bool sign_2 = Bget_sign(value_2);
  const bool result_sign = (sign_1 == minus || sign_2 == minus) &&
                           !(sign_1 == minus && sign_2 == minus);
  Bset_sign(&value_1, plus);
  Bset_sign(&value_2, plus);

  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R;                       // остаток reside
  big_decimal prev;
  scale_t new_scale = 0;
  bool overflow = false;
  uint8_t mul10_counter = 0;

  while (1) {
    for (uint8_t i = 0; i < 6; ++i) { /* сбрасывает значение Q */
      Q.bits[i] = 0;
    }
    R = (big_decimal){{0}};

    for (int i = BDIGITS_BIT_COUNT - 1; i >= 0; i--) {
      Bleft_shift(&R);
      Bset_bit(&R, 0, Bget_bit(value_1, i));
      if (Bdigits_ge(R, value_2)) {
        (void)Bdigits_sub(R, value_2, &R);
        Bset_bit(&Q, i, 1);
      }
    }
    if (mode == reside) {
      *result = R;
      return 0;
    }
    *result = Q;
    if (mode == whole) {
      return 0;
    }

    if ((mul10_counter >= 54) || (overflow)) {
      *result = prev;
      break;
    }

    if (!overflow) {
      prev = *result;
      overflow = (bool)Bdigits_mul10(&value_1);
      new_scale = Bget_scale(Q) + 1;
      Bset_scale(&Q, new_scale);
      mul10_counter += 1;
    }
  }
  Bset_sign(result, result_sign);
  return err_code;
}

// https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
err_t Bdigits_division(big_decimal value_1, big_decimal value_2,
                       big_decimal *result, uint8_t mode) {
  if (Bdigits_eq(value_2, (big_decimal){{0}})) /* если делим на 0 */
    return 3;

  err_t err_code = 0;
  const bool sign_1 = Bget_sign(value_1);
  const bool sign_2 = Bget_sign(value_2);
  const bool result_sign = (sign_1 == minus || sign_2 == minus) &&
                           !(sign_1 == minus && sign_2 == minus);
  Bset_sign(&value_1, plus);
  Bset_sign(&value_2, plus);

  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R;                       // остаток reside
  while (1) {
    for (uint8_t i = 0; i < 6; ++i) { /* сбрасывает значение Q */
      Q.bits[i] = 0;
    }
    R = (big_decimal){{0}};

    for (int i = BDIGITS_BIT_COUNT - 1; i >= 0; i--) {
      Bleft_shift(&R);
      Bset_bit(&R, 0, Bget_bit(value_1, i));
      if (Bdigits_ge(R, value_2)) {
        (void)Bdigits_sub(R, value_2, &R);
        Bset_bit(&Q, i, 1);
      }
    }
    if (mode == reside) {
      *result = R;
      return 0;
    }
    *result = Q;
    if (mode == whole) {
      return 0;
    }

    // домножаем на 10 и снова делим, пока остаток не станет 0.
    if (Bdigits_ne(R, (big_decimal){{0}})) {
      const bool overflow = Bdigits_mul10(&value_1);
      const scale_t new_scale = Bget_scale(Q) + 1;
      Bset_scale(&Q, new_scale);
      if (new_scale > MAX_SCALE) {
        err_code = 0;
        break;
      }
      if (overflow) { /* переполнение */
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
  Bset_sign(result, result_sign);
  return err_code;
}

int Bmod(big_decimal value_1, big_decimal value_2) {
  Bset_sign(&value_1, plus);
  Bset_sign(&value_2, plus);

  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R = (big_decimal){{0}};  // остаток reside

  for (uint8_t i = 0; i < 6; ++i) { /* сбрасывает значение Q */
    Q.bits[i] = 0;
  }
  R = (big_decimal){{0}};

  for (int i = BDIGITS_BIT_COUNT - 1; i >= 0; i--) {
    Bleft_shift(&R);
    Bset_bit(&R, 0, Bget_bit(value_1, i));
    if (Bdigits_ge(R, value_2)) {
      (void)Bdigits_sub(R, value_2, &R);
      Bset_bit(&Q, i, 1);
    }
  }

  return big_to_int(R);
}
