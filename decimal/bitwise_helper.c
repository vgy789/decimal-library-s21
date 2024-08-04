#include "s21_decimal.h"

bool get_bit(s21_decimal value, uint8_t bit_pos) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  return Bget_bit(big, bit_pos);
}

uint8_t get_scale(s21_decimal value) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  return Bget_scale(big);
}

bool get_sign(s21_decimal value) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  return Bget_sign(big);
}

void left_shift(s21_decimal *value) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bleft_shift(&big);
  big_to_decimal(big, value);
}

void set_bit(s21_decimal *value, uint8_t bit_pos, bool state) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bset_bit(&big, bit_pos, state);
  big_to_decimal(big, value);
}

bool set_scale(s21_decimal *value, uint8_t scale) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  const bool is_ok = Bset_scale(&big, scale);
  big_to_decimal(big, value);
  return is_ok;
}

void set_sign(s21_decimal *value, bool sign) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bset_sign(&big, sign);
  big_to_decimal(big, value);
}
