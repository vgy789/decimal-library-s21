#include "s21_decimal.h"

#include "bitwise_helper.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  uint8_t is_err = s21_mul(value, DECIMAL_ONE_MINUS, result);
  // TODO: Возвращает ошибку. О какой ошибке идёт речь?
  return is_err;
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
  s21_decimal value_1_buf = value_1;
  s21_decimal value_2_buf = value_2;
  const bool sign1_buf = get_sign(value_1);
  const bool sign2_buf = get_sign(value_2);

  if (sign1_buf == minus) {
    set_sign(&value_1, plus);
    compliment2(value_1, &value_1);
  }

  if (sign2_buf == minus) {
    set_sign(&value_2, plus);
    compliment2(value_2, &value_2);
  }

  /* сложение */
  s21_decimal buf = (s21_decimal){{0, 0, 0, 0}};
  bool transfer =
      add_word((uint32_t *)&buf.bits[0], value_1.bits[0], value_2.bits[0], 0);
  transfer = add_word((uint32_t *)&buf.bits[1], value_1.bits[1],
                      value_2.bits[1], transfer);
  transfer = add_word((uint32_t *)&buf.bits[2], value_1.bits[2],
                      value_2.bits[2], transfer);

  uint8_t err_code = 0;
  if (transfer) { /* проверка переполнения s21_decimal */
  }

  bool result_sign =
      (sign1_buf == minus && sign2_buf == minus); /* (-x) + (-y) */
  result_sign += (sign1_buf == minus &&
                  s21_is_greater(value_1_buf, value_2_buf)); /* (-x) + y */
  result_sign += (sign2_buf == minus &&
                  s21_is_less(value_1_buf, value_2_buf)); /* x + (-y) */
  if (result_sign) compliment2(buf, &buf);
  set_sign(&buf, result_sign);
  *result = buf;

  return err_code;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_negate(value_2, &value_2);
  uint8_t is_err = s21_add(value_1, value_2, result);
  // TODO: ошибка
  return is_err;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  const bool sign1 = get_sign(value_1);
  const bool sign2 = get_sign(value_2);
  if (sign1 == minus) compliment2(value_2, &value_1);
  if (sign2 == minus) compliment2(value_2, &value_2);

  uint8_t is_err = 0;
  *result = (s21_decimal){0b0};
  while (value_2.bits[0] | value_2.bits[1] | value_2.bits[2]) {
    s21_sub(value_2, (s21_decimal){{0b1, 0, 0, 0}}, &value_2);
    is_err = s21_add(value_1, *result, result);
  }

  const bool sign_result = sign1 ^ sign2;
  if (sign_result == minus) compliment2(*result, result);

  // TODO: ошибка
  return is_err;
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

int comparison(s21_decimal a,
               s21_decimal b) { //сравнивает числа с учетом знака и экспоненты
  int result = 2;
  bool sign_a = get_sign(a);
  bool sign_b = get_sign(b);
  if (sign_a > sign_b)
    result = 0; // а - отрицательное число, b - положительное
  if (sign_a < sign_b)
    result = 1; // b - отрицательное число, a - положительное
  if (sign_a == sign_b) {
    uint8_t scale_a = get_scale(a);
    uint8_t scale_b = get_scale(b);
    if (scale_a == scale_b)
      result = comparison_mantiss(a, b); // если экспоненты равны
      //a если они не равны, то обломитесь
  }
  return result; // 2 -> a==b, 1 -> a > b, 0 -> a < b
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  const bool result = comparison(value_1, value_2);
  return !result;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  const bool result = comparison(value_1, value_2);
  return result;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return comparison(value_1, value_2) == 2;
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  return comparison(value_1, value_2) == 1;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_greater(value_1, value_2);
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_less(value_1, value_2);
}
