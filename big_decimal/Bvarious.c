#include "big_decimal.h"

void Bswap(big_decimal *value_1, big_decimal *value_2) {
  big_decimal buf = *value_1;
  *value_1 = *value_2;
  *value_2 = buf;
}

int Bs21_negate(big_decimal value, big_decimal *result) {
  enum { minus_bit = 0x80000000 };
  uint8_t err_code =
      Bs21_mul(value, (big_decimal){{1, 0, 0, 0, 0, 0, minus_bit}}, result);
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
  return (value.bits[int_part] & (1 << bit_pos));
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

uint8_t Bget_scale(big_decimal value) {
  return (value.bits[6] & (0b11111111 << 16)) >> 16;
}

void Bset_scale(big_decimal *value, uint8_t scale) {
  const bool sign = Bget_sign(*value);

  value->bits[6] = (0b11111111 << 16) & scale << 16;
  Bset_sign(value, sign);
}

bool Bget_sign(big_decimal value) { return ((value.bits[6] >> 31) & 1); }

void Bset_sign(big_decimal *value, bool sign) {
  if (sign == plus)
    value->bits[6] &= ~(1 << 31);
  else
    value->bits[6] |= (1 << 31);
}

void Bleft_shift(big_decimal *value) {
  for (int32_t i = BDIGITS_BIT_COUNT - 1; i > 0; --i) {
    Bset_bit(value, i, Bget_bit(*value, i - 1));
  }
  Bset_bit(value, 0, 0);
}
