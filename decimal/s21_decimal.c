#include "s21_decimal.h"
#include "bitwise_helper.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  const bool sign = get_sign(value);
  uint8_t bit_i = 0;

  while (bit_i < 96) {
    const bool bit = get_bit(value, bit_i);
    set_bit(result, bit_i, !bit);
    bit_i += 1;
  }
  const bool is_err = s21_add(*result, (s21_decimal){{0b1, 0, 0, 0}}, result);
  set_sign(result, !sign);
  // TODO: Возвращает ошибку. О какой ошибке идёт речь?
  return is_err;
}

int s21_add(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  uint8_t bit_i = 0;
  bool bit_transfer = false;

  while (bit_i < 96) {
    const bool bit1 = get_bit(value1, bit_i);
    const bool bit2 = get_bit(value2, bit_i);
    bool bit_result = bit1 ^ bit2 ^ bit_transfer;
    set_bit(result, bit_i, bit_result);
    bit_transfer = (bit1 & bit2) | (bit1 & bit_transfer) | (bit2 & bit_transfer);

    bit_i += 1;
  }

  // TODO: переполнение, установка знака у результата при переполнениии. Ошибка при переполнении.

  // const bool err_overflow = bit_transfer == true && !(get_sign(value1) ^ get_sign(value2));
  // if (err_overflow) { /* test overflow */
  //   printf("ERROR\n");
  // }
  // TODO: ошибка
  return 0;
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
  if (sign1 == minus)
    s21_negate(value2, &value1);
  if (sign2 == minus)
    s21_negate(value2, &value2);

  uint8_t is_err = 0;
  *result = (s21_decimal){0b0};
  while(value2.bits[0] | value2.bits[1] | value2.bits[2]) {
    s21_sub(value2, (s21_decimal){{0b1}}, &value2);
    is_err = s21_add(value1, *result, result);
  }

  const bool sign_result = sign1 ^ sign2;
  if (sign_result == minus)
    s21_negate(*result, result);
  
  // TODO: ошибка
  return is_err;
}

// int s21_div(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  // нужна операция сравнения >=
// }

bool set_scale(s21_decimal *value, uint8_t scale) {
  if (scale > 28) return 1;
  s21_decimal decimal_scale = {{scale, 0, 0, 0}};

  for (int bit_i = scale_start; bit_i <= scale_end; ++bit_i)
    set_bit(value, bit_i, get_bit(decimal_scale, bit_i % scale_start));
  return 0;
}

unsigned get_scale(s21_decimal value) {
  return (value.bits[3] & (0b11111111 << 16)) >> 16;
}

void set_bit(s21_decimal *value, uint8_t bit_pos, bool status) {
  uint8_t int_part = 0;

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  if (status == false) {
    value->bits[int_part] &= ~(1 << bit_pos);
  } else {
    value->bits[int_part] |= (1 << bit_pos);
  }
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

void decimal_binary(s21_decimal value) {
  if (get_sign(value))
    printf("-");
  else
    printf("+");
  uint_binary(value.bits[2]);
  uint_binary(value.bits[1]);
  uint_binary(value.bits[0]);
}

void set_sign(s21_decimal *value, bool sign) {
  if (sign == plus) {
    value->bits[3] &= ~(1 << 31);
  } else {
    value->bits[3] |= (1 << 31);
  }
}

bool get_sign(s21_decimal value) { return ((value.bits[3] >> 31) & 1); }
