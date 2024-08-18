#include "big_decimal.h"

bool Bget_bit(big_decimal value, uint8_t bit_pos) {
  uint8_t int_part = 0; /* индекс ячейки в массиве */

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  return (value.bits[int_part] & (1U << bit_pos));
}

void Bset_bit(big_decimal *value, uint8_t bit_pos, bool state) {
  uint8_t int_part = 0; /* индекс ячейки в массиве */

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  if (state == false)
    value->bits[int_part] &= ~(1U << bit_pos);
  else
    value->bits[int_part] |= (1U << bit_pos);
}

scale_t Bget_scale(big_decimal value) {
  return (value.bits[6] & (255U << 16)) >> 16;
}

void Bset_scale(big_decimal *value, scale_t scale) {
  const bool sign = Bget_sign(*value);
  value->bits[6] = (255U << 16) & scale << 16;
  Bset_sign(value, sign);
}

bool Bget_sign(big_decimal value) { return Bget_bit(value, 223); }

void Bset_sign(big_decimal *value, bool sign) {
  if (sign == plus)
    value->bits[6] &= ~(1U << 31);
  else
    value->bits[6] |= (1U << 31);
}

err_t Bleft_shift(big_decimal *value) {
  err_t is_overflow = Bget_bit(*value, BDIGITS_BIT_COUNT - 1) == 1;

  for (int16_t i = BDIGITS_BIT_COUNT - 1; i > 0; --i) {
    Bset_bit(value, i, Bget_bit(*value, i - 1));
  }
  Bset_bit(value, 0, 0);
  return is_overflow;
}

err_t Bright_shift(big_decimal *value) {
  err_t is_overflow = Bget_bit(*value, 0) == 1;

  for (int16_t i = 1; i < BDIGITS_BIT_COUNT - 1; ++i) {
    Bset_bit(value, i - 1, Bget_bit(*value, i));
  }
  Bset_bit(value, BDIGITS_BIT_COUNT - 1, 0);
  return is_overflow;
}
