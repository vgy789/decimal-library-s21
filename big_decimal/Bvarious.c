#include "big_decimal.h"

void Bswap(big_decimal *value_1, big_decimal *value_2) {
  big_decimal buf = *value_1;
  *value_1 = *value_2;
  *value_2 = buf;
}

err_t Bs21_negate(big_decimal value, big_decimal *result) {
  enum { minus_bit = 0x80000000 };
  err_t err_code =
      Bdigits_mul(value, (big_decimal){{1, 0, 0, 0, 0, 0, minus_bit}}, result);
  return err_code;
}

void Bset_result_sign(big_decimal *value, bool sign) {
  if (Bdigits_eq(*value, (big_decimal){{0}})) { /* -0 to +0 */
    Bset_sign(value, plus);
  } else {
    Bset_sign(value, sign);
  }
}

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

uint8_t Bget_scale(big_decimal value) {
  return (value.bits[6] & (0b11111111 << 16)) >> 16;
}

void Bset_scale(big_decimal *value, scale_t scale) {
  const bool sign = Bget_sign(*value);
  value->bits[6] = (0b11111111 << 16) & scale << 16;
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
