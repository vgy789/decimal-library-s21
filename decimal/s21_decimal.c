#include "s21_decimal.h"

#include "bitwise_helper.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  uint8_t is_err = s21_mul(value, DECIMAL_ONE_MINUS, result);
  return is_err;
}

uint8_t compliment2(s21_decimal value, s21_decimal *result) {
  value.bits[0] = ~value.bits[0];
  value.bits[1] = ~value.bits[1];
  value.bits[2] = ~value.bits[2];
  s21_inc(value, result);
  // TODO: Возвращает ошибку. О какой ошибке идёт речь?
  return 0;
}

static bool add_word(uint32_t *result, uint32_t value1, uint32_t value2,
                     bool transfer) {
  bool overflow = 0;
  overflow += __builtin_uadd_overflow(value1, value2, result);
  overflow += __builtin_uadd_overflow(*result, transfer, result);
  return overflow;
}

int s21_add(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  bool sign1_buf = get_sign(value1);
  bool sign2_buf = get_sign(value2);

  if (sign1_buf == minus) {
    set_sign(&value1, plus);
    compliment2(value1, &value1);
  }
  if (get_sign(value2) == minus) {
    set_sign(&value2, plus);
    compliment2(value2, &value2);
  }

  s21_decimal buf = (s21_decimal){{0, 0, 0, 0}};
  bool transfer = add_word(&buf.bits[0], value1.bits[0], value2.bits[0], 0);
  transfer = add_word(&buf.bits[1], value1.bits[1], value2.bits[1], transfer);
  transfer = add_word(&buf.bits[2], value1.bits[2], value2.bits[2], transfer);

  uint8_t err_code = 0;
  if (transfer) { /* переполнение | ошибка */
  }

  bool result_sign =
      (sign1_buf == minus && sign2_buf == minus); /* (-x) + (-y) */
  if (result_sign) {
    compliment2(buf, &buf);
  }
  set_sign(&buf, result_sign);
  *result = buf;

  return err_code;
}

int s21_sub(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  s21_negate(value2, &value2);
  uint8_t is_err = s21_add(value1, value2, result);
  // TODO: ошибка
  return is_err;
}

int s21_mul(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  const bool sign1 = get_sign(value1);
  const bool sign2 = get_sign(value2);
  if (sign1 == minus) compliment2(value2, &value1);
  if (sign2 == minus) compliment2(value2, &value2);

  uint8_t is_err = 0;
  *result = (s21_decimal){0b0};
  while (value2.bits[0] | value2.bits[1] | value2.bits[2]) {
    s21_sub(value2, (s21_decimal){{0b1, 0, 0, 0}}, &value2);
    is_err = s21_add(value1, *result, result);
  }

  const bool sign_result = sign1 ^ sign2;
  if (sign_result == minus) compliment2(*result, result);

  // TODO: ошибка
  return is_err;
}

// int s21_div(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
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

  // value = abs(value);
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
