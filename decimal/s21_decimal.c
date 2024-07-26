#include "s21_decimal.h"

#include "bitwise_helper.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  enum { minus_bit = 0x80000000 };
  uint8_t err_code =
      s21_mul(value, (s21_decimal){{1, 0, 0, minus_bit}}, result);
  return err_code;
}

void compliment2(s21_decimal value, s21_decimal *result) {
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
  s21_decimal value_1_orig = value_1;
  s21_decimal value_2_orig = value_2;
  const bool sign_1_orig = get_sign(value_1);
  const bool sign_2_orig = get_sign(value_2);

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
  s21_decimal buf = (s21_decimal){{0}};
  bool transfer =
      add_word((uint32_t *)&buf.bits[0], value_1.bits[0], value_2.bits[0], 0);
  transfer = add_word((uint32_t *)&buf.bits[1], value_1.bits[1],
                      value_2.bits[1], transfer);
  transfer = add_word((uint32_t *)&buf.bits[2], value_1.bits[2],
                      value_2.bits[2], transfer);

  set_sign(&value_1_orig, plus);  // далее нужно сравнить биты не учитывая знак
  set_sign(&value_2_orig, plus);
  bool result_sign =
      (sign_1_orig == minus && sign_2_orig == minus) ||
      (sign_1_orig == minus && s21_is_greater(value_1_orig, value_2_orig)) ||
      (sign_2_orig == minus && s21_is_less(value_1_orig, value_2_orig));
  if (result_sign) compliment2(buf, &buf);
  set_sign(&buf, result_sign);
  *result = buf;

  uint8_t err_code = 0;
  if (transfer == true) { /* проверка переполнения s21_decimal */
  }

  return err_code;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const bool sign = get_sign(value_2);
  set_sign(&value_2, !sign);
  uint8_t err_code = s21_add(value_1, value_2, result);
  // TODO: ошибка
  return err_code;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const bool sign_1_orig = get_sign(value_1);
  const bool sign_2_orig = get_sign(value_2);

  if (sign_1_orig == minus) set_sign(&value_1, plus);
  if (sign_2_orig == minus) set_sign(&value_2, plus);

  uint8_t err_code = 0;
  *result = (s21_decimal){0};
  while (value_2.bits[0] | value_2.bits[1] | value_2.bits[2]) {
    (void)s21_sub(value_2, (s21_decimal){{1, 0, 0, 0}}, &value_2);
    err_code = s21_add(value_1, *result, result);
  }

  const bool result_sign = (sign_1_orig == minus || sign_2_orig == minus) &&
                           !(sign_1_orig == minus && sign_2_orig == minus);
  set_sign(result, result_sign);

  // TODO: ошибка
  return err_code;
}

// int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
// нужна операция сравнения >=
// }

bool set_scale(s21_decimal *value, uint8_t scale) {
  if (scale > 28) return 1;
  s21_decimal decimal_scale = (s21_decimal){{scale, 0, 0, 0}};

  for (int bit_i = scale_start; bit_i <= scale_end; ++bit_i)
    set_bit(value, bit_i, get_bit(decimal_scale, bit_i % scale_start));
  return 0;
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

int comparison_mantiss(s21_decimal value_1, s21_decimal value_2) {
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
