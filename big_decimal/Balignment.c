#include "big_decimal.h"

/* делится ли число нацело */
static bool is_Bmagnitude_div10(big_decimal value) {
  big_decimal value_10 = (big_decimal){{10, 0, 0, 0, 0, 0}};
  Bset_sign(&value, plus);
  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R = (big_decimal){{0}};  // остаток remainder

  while (1) {
    for (u_int8_t i = 0; i < 6; ++i) { /* сбрасывает значение Q */
      Q.bits[i] = 0;
    }
    R = (big_decimal){{0}};

    for (int i = BMAGNITUDE_BIT_COUNT - 1; i >= 0; i--) {
      Bleft_shift(&R);
      Bset_bit(&R, 0, Bget_bit(value, i));
      if (Bmagnitude_ge(R, value_10)) {
        (void)Bmagnitude_sub(R, value_10, &R);
        Bset_bit(&Q, i, 1);
      }
    }
    if (Bmagnitude_ne(R, (big_decimal){{0}})) {
      return 1;
    } else {
      break;
    }
  }

  return 0;
}

void Bcircumcision(big_decimal *value) {
  int8_t mant_size = Bget_scale(*value);

  while (mant_size > 0 && is_Bmagnitude_div10(*value) == 0) {
    Bmagnitude_div10(value);
    mant_size--;
  }
  Bset_scale(value, mant_size);
}

void Balignment(big_decimal *value_1, big_decimal *value_2) {
  uint8_t mant_size1 = Bget_scale(*value_1);
  uint8_t mant_size2 = Bget_scale(*value_2);

  if (Bmagnitude_eq(*value_1, *value_2)) {
    if (mant_size1 > mant_size2) {
      mant_size2 = mant_size1;
    } else {
      mant_size1 = mant_size2;
    }
    Bset_scale(value_1, mant_size1);
    Bset_scale(value_2, mant_size2);
    return;
  }

  if (mant_size1 == mant_size2) return;
  if (mant_size1 > mant_size2) {
    Balignment(value_2, value_1);
    return;
  }

  while (mant_size1 < mant_size2) {
    Bmagnitude_mul10(value_1);
    mant_size1++;
  }

  Bset_scale(value_1, mant_size1);
  while (mant_size1 < mant_size2) {
    if ((*value_2).bits[0] % 10 != 0) break;
    Bmagnitude_div10(value_2);
    mant_size2--;
  }

  Bset_scale(value_2, mant_size2);
  while (mant_size1 < mant_size2) {
    Bmagnitude_div10(value_2);
    mant_size2--;
  }
  Bset_scale(value_2, mant_size2);
}
