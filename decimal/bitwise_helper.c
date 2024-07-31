#include "s21_decimal.h"

bool get_bit(s21_decimal value, uint8_t bit_pos) {
  uint8_t int_part = 0;

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  return (value.bits[int_part] & (1 << bit_pos));
};

uint8_t get_scale(s21_decimal value) {
  return (value.bits[3] & (0b11111111 << 16)) >> 16;
}

bool get_sign(s21_decimal value) { return ((value.bits[3] >> 31) & 1); }

void left_shift(s21_decimal *value) {
  for (int i = 95; i > 0; --i) {
    set_bit(value, i, get_bit(*value, i - 1));
  }
  set_bit(value, 0, 0);
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

bool set_scale(s21_decimal *value, uint8_t scale) {
  if (scale > 28) return 0;
  bool sign = get_sign(*value);

  value->bits[3] = (0b11111111 << 16) & scale << 16;
  set_sign(value, sign);

  return 1;
}

void set_sign(s21_decimal *value, bool sign) {
  if (sign == plus)
    value->bits[3] &= ~(1 << 31);
  else
    value->bits[3] |= (1 << 31);
}
