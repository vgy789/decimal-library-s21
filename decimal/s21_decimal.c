#include "s21_decimal.h"

#include <math.h>

#include "bitwise_helper.h"

static void left_shift(s21_decimal *value) {
  for (int i = 95; i > 0; --i) {
    set_bit(value, i, get_bit(*value, i - 1));
  }
  set_bit(value, 0, 0);
}

static bool mul_by_ten(s21_decimal *value) {
  uint8_t scale = get_scale(*value);
  bool is_err = s21_mul(*value, (s21_decimal){{10, 0, 0, 0}}, value);
  set_scale(value, ++scale);
  return is_err;
}

static bool div_by_ten(s21_decimal *value) {
  uint8_t scale = get_scale(*value);
  bool is_err = s21_div(*value, (s21_decimal){{10, 0, 0, 0}}, value);
  set_scale(value, ++scale);
  return is_err;
}

int get_first_num(int value, int *size) {
  int rez = 0;
  *size = 0;
  while (value > 0) {
    rez = value % 10;
    value /= 10;
    (*size)++;
  }
  (*size) -= 1;
  return rez;
}

void alignment(s21_decimal *value1, s21_decimal *value2) {
  int mant_size1 = get_scale(*value1), mant_size2 = get_scale(*value2);
  if (mant_size1 == mant_size2) return;
  if (mant_size2 < mant_size1) {
    alignment(value2, value1);
    return;
  }
  while (mant_size1 < mant_size2) {
    if ((*value1).bits[2] > 0xFFFFFFF / 10) break;
    mul_by_ten(value1);
    mant_size1++;
  }

  set_scale(value1, mant_size1);
  while (mant_size1 < mant_size2) {
    if ((*value2).bits[0] % 10 != 0) break;
    div_by_ten(value2);
    mant_size2--;
  }

  set_scale(value2, mant_size2);
  while (mant_size1 < mant_size2) {
    div_by_ten(value2);
    mant_size2--;
  }
  set_scale(value2, mant_size2);
}

///
///
///
///

int s21_negate(s21_decimal value, s21_decimal *result) {
  enum { minus_bit = 0x80000000 };
  uint8_t err_code =
      s21_mul(value, (s21_decimal){{1, 0, 0, minus_bit}}, result);
  return err_code;
}

// Перевод мантиссы в дополнительный код (дополнение до двух).
static void compliment2(s21_decimal value, s21_decimal *result) {
  value.bits[0] = ~value.bits[0];
  value.bits[1] = ~value.bits[1];
  value.bits[2] = ~value.bits[2];
  s21_inc(value, result);
}

static bool add_word(uint32_t *result, uint32_t value_1, uint32_t value_2,
                     bool transfer) {
  bool overflow = 0;
  overflow += __builtin_uadd_overflow(value_1, value_2, result);
  overflow += __builtin_uadd_overflow(*result, transfer, result);
  return overflow;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  *result = (s21_decimal){{0, 0, 0, 0}};
  s21_decimal value1_orig = value_1;
  s21_decimal value2_orig = value_2;
  const bool sign_1_orig = get_sign(value_1);
  const bool sign_2_orig = get_sign(value_2);

  /* чтобы узнать знак результата необходимо сравнить биты не учитывая знак */
  set_sign(&value_1, plus);
  set_sign(&value_2, plus);
  bool result_sign =
      (sign_1_orig == minus && sign_2_orig == minus) ||
      (sign_1_orig == minus && s21_is_greater(value_1, value_2)) ||
      (sign_2_orig == minus && s21_is_less(value_1, value_2));

  // при необходимости переводим в дополнительный код.
  if (sign_1_orig == minus) {
    set_sign(&value_1, plus);  // complement2 использует сложение. Чтобы не уйти
                               // в рекурсию ставлю знак в +.
    compliment2(value_1, &value_1);
  }
  if (sign_2_orig == minus) {
    set_sign(&value_2, plus);
    compliment2(value_2, &value_2);
  }

  /* сложение */
  bool transfer = add_word((uint32_t *)&result->bits[0], value_1.bits[0],
                           value_2.bits[0], 0);
  transfer = add_word((uint32_t *)&result->bits[1], value_1.bits[1],
                      value_2.bits[1], transfer);
  transfer = add_word((uint32_t *)&result->bits[2], value_1.bits[2],
                      value_2.bits[2], transfer);

  uint8_t err_code = 0;
  if (result_sign == minus) {
    compliment2(*result, result);
  }

  if (sign_1_orig == sign_2_orig) { /* проверка переполнения s21_decimal */
    set_sign(&value1_orig, plus);
    set_sign(&value2_orig, plus);
    // если результат суммы положительных/отрицательных меньше/больше, значит
    // ошибка переполнения.
    if (s21_is_greater(value1_orig, *result) ||
        s21_is_greater(value2_orig, *result)) {
      err_code = 1;
      if (result_sign) {
        err_code = 2;
      }
    }
  }
  set_sign(result, result_sign);
  return err_code;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const bool sign = get_sign(value_2);
  // инвертируем знак и складываем
  set_sign(&value_2, !sign);
  uint8_t err_code = s21_add(value_1, value_2, result);
  // TODO: ошибка
  return err_code;
}

static void swap(s21_decimal *value_1, s21_decimal *value_2) {
  s21_decimal buf = *value_1;
  *value_1 = *value_2;
  *value_2 = buf;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const bool sign_1_orig = get_sign(value_1);
  const bool sign_2_orig = get_sign(value_2);

  const bool result_sign = (sign_1_orig == minus || sign_2_orig == minus) &&
                           !(sign_1_orig == minus && sign_2_orig == minus);
  {
    // оптимизация умножения :)
    set_sign(&value_1, plus);
    set_sign(&value_2, plus);
    if (s21_is_greater(value_2, value_1)) {
      swap(&value_1, &value_2);
    }
    set_sign(&value_1, sign_1_orig);
    set_sign(&value_2, sign_2_orig);
  }

  if (sign_1_orig == minus) set_sign(&value_1, plus);
  if (sign_2_orig == minus) set_sign(&value_2, plus);

  uint8_t err_code = 0;
  *result = (s21_decimal){0};
  // умножение через сложение
  while (value_2.bits[0] | value_2.bits[1] | value_2.bits[2]) {
    (void)s21_sub(value_2, (s21_decimal){{1, 0, 0, 0}}, &value_2);
    err_code = s21_add(value_1, *result, result);
    if (err_code) {
      err_code = 1;
      if (result_sign == minus) err_code = 2;
      return err_code;
    }
  }

  set_sign(result, result_sign);

  // TODO: ошибка
  return err_code;
}

// https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (s21_is_equal(value_2, (s21_decimal){{0}})) /* если делим на 0 */
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

    int n = 96;  // number of bits in value_1
    for (int i = n - 1; i >= 0; i--) {
      left_shift(&R);
      set_bit(&R, 0, get_bit(value_1, i));
      if (s21_is_greater_or_equal(R, value_2)) {
        (void)s21_sub(R, value_2, &R);
        set_bit(&Q, i, 1);
      }
    }

    *result = Q;

    // домножаем на 10 и снова делим, пока остаток не станет 0
    if (s21_is_not_equal(R, (s21_decimal){{0}})) {
      const bool overflow = mul_by_ten(&value_1);
      set_scale(&Q, get_scale(Q) + 1);
      if (overflow) { /* слишком много цифр после запятой */
        break;
      }
    } else {
      break;
    }
  }
  set_sign(result, result_sign);
  return 0;
}

bool set_scale(s21_decimal *value, uint8_t scale) {
  if (scale > 28) return 0;
  bool sign = get_sign(*value);

  value->bits[3] = (0b11111111 << 16) & scale << 16;
  set_sign(value, sign);

  return 1;
}

uint8_t get_scale(s21_decimal value) {
  return (value.bits[3] & (0b11111111 << 16)) >> 16;
}

void set_bit(s21_decimal *value, uint8_t bit_pos, bool state) {
  uint8_t int_part = 0;

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  if (state == false)
    value->bits[int_part] &= ~(1 << bit_pos);
  else
    value->bits[int_part] |= (1 << bit_pos);
}

bool get_bit(s21_decimal value, uint8_t bit_pos) {
  uint8_t int_part = 0;

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  return (value.bits[int_part] & (1 << bit_pos));
};

void uint_binary(uint32_t value) {
  int8_t bits = 31;

  while (value != 0) {
    int buf = value - (1 << bits);

    if (buf >= 0) {
      value = buf;
      printf("1");
    } else {
      printf("0");
    }
    --bits;
  }
  while (bits >= 0) {
    printf("0");
    --bits;
  }
}

void decimal_binary(s21_decimal value, bool print_scale) {
  if (get_sign(value))
    printf("(-)");
  else
    printf("(+)");

  if (print_scale) {
    uint_binary(value.bits[3]);
    printf("\'");
  }
  uint_binary(value.bits[2]);
  printf("\'");
  uint_binary(value.bits[1]);
  printf("\'");
  uint_binary(value.bits[0]);
  printf("(scale=%d)", get_scale(value));
}

void set_sign(s21_decimal *value, bool sign) {
  if (sign == plus)
    value->bits[3] &= ~(1 << 31);
  else
    value->bits[3] |= (1 << 31);
}

bool get_sign(s21_decimal value) { return ((value.bits[3] >> 31) & 1); }

int s21_inc(s21_decimal value, s21_decimal *result) {
  const s21_decimal one = (s21_decimal){{1, 0, 0, 0}};
  s21_add(value, one, result);
  return 0;
}

int s21_dec(s21_decimal value, s21_decimal *result) {
  const s21_decimal one = (s21_decimal){{1, 0, 0, 0}};
  s21_sub(value, one, result);
  return 0;
}

static int comparison_mantiss(s21_decimal value_1, s21_decimal value_2) {
  int bit_pos = 96;
  bool pos_a, pos_b;
  int result = 0;
  while (bit_pos >= 0) {
    pos_a = get_bit(value_1, bit_pos);
    pos_b = get_bit(value_2, bit_pos);
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
  return result;  // 0 -> a==b, 1 -> a > b, 2 -> a < b
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  const bool result = comparison_mantiss(value_1, value_2);
  return !result;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  const bool result = comparison_mantiss(value_1, value_2);
  return result;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return comparison_mantiss(value_1, value_2) == 2;
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  return comparison_mantiss(value_1, value_2) == 1;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_greater(value_1, value_2);
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_less(value_1, value_2);
}
