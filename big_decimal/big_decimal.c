#include "big_decimal.h"

int Bs21_negate(big_decimal value, big_decimal *result) {
  enum { minus_bit = 0x80000000 };
  uint8_t err_code =
      Bs21_mul(value, (big_decimal){{1, 0, 0, 0, 0, 0, minus_bit}}, result);
  return err_code;
}

static bool Badd_word(uint32_t *result, uint32_t value_1, uint32_t value_2,
                      bool transfer) {
  bool overflow = 0;
  overflow += __builtin_uadd_overflow(value_1, value_2, result);
  overflow += __builtin_uadd_overflow(*result, transfer, result);
  return overflow;
}

static void Bset_result_sign(big_decimal *value, bool sign) {
  if (Bmantiss_eq(*value, (big_decimal){{0}})) { /* -0 to +0 */
    Bset_sign(value, plus);
  } else {
    Bset_sign(value, sign);
  }
}

int Bs21_add(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  *result = (big_decimal){{0}};
  big_decimal value1_orig = value_1;
  big_decimal value2_orig = value_2;
  const bool sign_1_orig = Bget_sign(value_1);
  const bool sign_2_orig = Bget_sign(value_2);

  /* чтобы узнать знак результата необходимо сравнить биты не учитывая знак. */
  Bset_sign(&value_1, plus);
  Bset_sign(&value_2, plus);
  bool result_sign = (sign_1_orig == minus && sign_2_orig == minus) ||
                     (sign_1_orig == minus && Bmantiss_gt(value_1, value_2)) ||
                     (sign_2_orig == minus && Bmantiss_lt(value_1, value_2));

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

  if (sign_1_orig == sign_2_orig) { /* проверка переполнения s21_decimal */
    // если результат суммы положительных/отрицательных меньше/больше, значит
    // ошибка переполнения.
    if (Bmantiss_gt(value1_orig, *result) ||
        Bmantiss_gt(value2_orig, *result)) {
      err_code = 1;
      if (result_sign) {
        err_code = 2;
      }
    }
  }
  Bset_result_sign(result, result_sign);

  return err_code;
}

int Bs21_sub(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  const bool sign = Bget_sign(value_2);
  // инвертируем знак и складываем
  Bset_sign(&value_2, !sign);
  uint8_t err_code = Bs21_add(value_1, value_2, result);
  return err_code;
}

static void Bswap(big_decimal *value_1, big_decimal *value_2) {
  big_decimal buf = *value_1;
  *value_1 = *value_2;
  *value_2 = buf;
}

int Bs21_mul(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  const bool sign_1_orig = Bget_sign(value_1);
  const bool sign_2_orig = Bget_sign(value_2);

  const bool result_sign = (sign_1_orig == minus || sign_2_orig == minus) &&
                           !(sign_1_orig == minus && sign_2_orig == minus);
  {
    // оптимизация умножения :-)
    if (Bmantiss_gt(value_2, value_1)) {
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
  while (Bmantiss_ne(value_2, (big_decimal){{0}})) {
    Bs21_dec(value_2, &value_2);
    err_code = Bs21_add(value_1, *result, result);
    if (err_code) {
      err_code = 1;
      if (result_sign == minus) err_code = 2;
      return err_code;
    }
  }
  Bset_result_sign(result, result_sign);

  return err_code;
}

// https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
int Bs21_div(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  if (Bmantiss_eq(value_2, (big_decimal){{0}})) /* если делим на 0 */
    return 2;

  const bool sign_1 = Bget_sign(value_1);
  const bool sign_2 = Bget_sign(value_2);
  const bool result_sign = (sign_1 == minus || sign_2 == minus) &&
                           !(sign_1 == minus && sign_2 == minus);
  Bset_sign(&value_1, plus);
  Bset_sign(&value_2, plus);

  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R = (big_decimal){{0}};  // остаток remainder
  while (1) {
    Q.bits[0] = 0;
    Q.bits[1] = 0;
    Q.bits[2] = 0;
    R = (big_decimal){{0}};

    for (int i = BMANTISS_BIT_COUNT - 1; i >= 0; i--) {
      Bleft_shift(&R);
      Bset_bit(&R, 0, Bget_bit(value_1, i));
      if (Bmantiss_ge(R, value_2)) {
        (void)Bs21_sub(R, value_2, &R);
        Bset_bit(&Q, i, 1);
      }
    }

    *result = Q;

    // домножаем на 10 и снова делим, пока остаток не станет 0.
    if (Bmantiss_ne(R, (big_decimal){{0}})) {
      const bool overflow = Bmul_by_ten(&value_1);
      Bset_scale(&Q, Bget_scale(Q) + 1);
      if (overflow) { /* слишком много цифр после запятой */
        break;
      }
    } else {
      break;
    }
  }
  Bset_result_sign(result, result_sign);

  return 0;
}

// сравнивает мантиссы без учёта знака и scale
static int Bcomparison_mantiss(big_decimal value_1, big_decimal value_2) {
  int16_t bit_pos = BMANTISS_BIT_COUNT - 1;
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

int Bmantiss_eq(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison_mantiss(value_1, value_2);
  return !result;
}

int Bmantiss_ne(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison_mantiss(value_1, value_2);
  return result;
}

int Bmantiss_lt(big_decimal value_1, big_decimal value_2) {
  return Bcomparison_mantiss(value_1, value_2) == 2;
}

int Bmantiss_gt(big_decimal value_1, big_decimal value_2) {
  return Bcomparison_mantiss(value_1, value_2) == 1;
}

int Bmantiss_le(big_decimal value_1, big_decimal value_2) {
  return !Bmantiss_gt(value_1, value_2);
}

int Bmantiss_ge(big_decimal value_1, big_decimal value_2) {
  return !Bmantiss_lt(value_1, value_2);
}

int Bs21_is_equal(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison(value_1, value_2);
  return !result;
}

int Bs21_is_not_equal(big_decimal value_1, big_decimal value_2) {
  const bool result = Bcomparison(value_1, value_2);
  return result;
}

int Bs21_is_less(big_decimal value_1, big_decimal value_2) {
  return Bcomparison(value_1, value_2) == 2;
}

int Bs21_is_greater(big_decimal value_1, big_decimal value_2) {
  return Bcomparison(value_1, value_2) == 1;
}

int Bs21_is_less_or_equal(big_decimal value_1, big_decimal value_2) {
  return !Bs21_is_greater(value_1, value_2);
}

int Bs21_is_greater_or_equal(big_decimal value_1, big_decimal value_2) {
  return !Bs21_is_less(value_1, value_2);
}

int Bs21_inc(big_decimal value, big_decimal *result) {
  const big_decimal one = (big_decimal){{1, 0, 0, 0, 0, 0, 0}};
  Bs21_add(value, one, result);
  return 0;
}

int Bs21_dec(big_decimal value, big_decimal *result) {
  const big_decimal one = (big_decimal){{1, 0, 0, 0, 0, 0, 0}};
  Bs21_sub(value, one, result);
  return 0;
}
