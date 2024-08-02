#include "big_decimal.h"

bool Bget_bit(big_decimal value, uint8_t bit_pos) {
  uint8_t int_part = 0; /* индекс ячейки в массиве */

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  return (value.bits[int_part] & (1 << bit_pos));
}

uint8_t Bget_scale(big_decimal value) {
  return (value.bits[6] & (0b11111111 << 16)) >> 16;
}

bool Bget_sign(big_decimal value) { return ((value.bits[6] >> 31) & 1); }

void Bleft_shift(big_decimal *value) {
  for (int32_t i = BMANTISS_BIT_COUNT - 1; i > 0; --i) {
    Bset_bit(value, i, Bget_bit(*value, i - 1));
  }
  Bset_bit(value, 0, 0);
}

void Bset_bit(big_decimal *value, uint8_t bit_pos, bool state) {
  uint8_t int_part = 0; /* индекс ячейки в массиве */

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  if (state == false)
    value->bits[int_part] &= ~(1 << bit_pos);
  else
    value->bits[int_part] |= (1 << bit_pos);
}

bool Bset_scale(big_decimal *value, uint8_t scale) {
  if (scale > 28) return 0;
  bool sign = Bget_sign(*value);

  value->bits[6] = (0b11111111 << 16) & scale << 16;
  Bset_sign(value, sign);

  return 1;
}

void Bset_sign(big_decimal *value, bool sign) {
  if (sign == plus)
    value->bits[6] &= ~(1 << 31);
  else
    value->bits[6] |= (1 << 31);
}
