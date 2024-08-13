#include "big_decimal.h"

/* делится ли число нацело */
static bool is_Bdigits_div10(big_decimal value) {
  big_decimal value_10 = (big_decimal){{10, 0, 0, 0, 0, 0}};
  Bset_sign(&value, plus);
  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R;                       // остаток reside

  while (1) {
    for (uint8_t i = 0; i < 6; ++i) { /* сбрасывает значение Q */
      Q.bits[i] = 0;
    }
    R = (big_decimal){{0}};

    for (int i = BDIGITS_BIT_COUNT - 1; i >= 0; i--) {
      Bleft_shift(&R);
      Bset_bit(&R, 0, Bget_bit(value, i));
      if (Bdigits_ge(R, value_10)) {
        (void)Bdigits_sub(R, value_10, &R);
        Bset_bit(&Q, i, 1);
      }
    }
    if (Bdigits_ne(R, (big_decimal){{0}})) {
      return 1;
    } else {
      break;
    }
  }

  return 0;
}

void Bnormalize_recursive(big_decimal *value, scale_t scale) {
  if (scale < 0) {
    /* дополняем нулями */
    Bdigits_mul10(value);
    Bnormalize_recursive(value, scale + 1);
  } else if (scale > 0 && is_Bdigits_div10(*value) == 0) {
    /* обрезаем лишние нули */
    Bdigits_div10(value);
    Bnormalize_recursive(value, scale - 1);
  } else {
    Bset_scale(value, scale);
  }
}

void Bnormalize(big_decimal *value) {
  scale_t scale = Bget_scale(*value);
  Bnormalize_recursive(value, scale);
}

void Balignment(big_decimal *value_1, big_decimal *value_2, bool for_add) {
  scale_t scale_1 = Bget_scale(*value_1);
  scale_t scale_2 = Bget_scale(*value_2);

  if (for_add) {
    if (Bdigits_eq(*value_1, *value_2)) {
      if (scale_1 > scale_2) {
        scale_2 = scale_1;
      } else {
        scale_1 = scale_2;
      }
      Bset_scale(value_1, scale_1);
      Bset_scale(value_2, scale_2);
      return;
    }
  }

  if (scale_1 == scale_2) return;
  if (scale_1 > scale_2) {
    Balignment(value_2, value_1, for_add);
    return;
  }

  while (scale_1 < scale_2) {
    Bdigits_mul10(value_1);
    scale_1++;
  }

  Bset_scale(value_1, scale_1);

  Bset_scale(value_2, scale_2);
  while (scale_1 < scale_2) {
    Bdigits_div10(value_2);
    scale_2--;
  }
  Bset_scale(value_2, scale_2);
}
