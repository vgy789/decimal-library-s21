#include <math.h>

#include "../test/debug_helper.h"
#include "big_decimal.h"

bool Bmantiss_mul10(big_decimal *value) {
  // const uint8_t scale = Bget_scale(*value);
  const uint8_t err_code =
      Bs21_mul(*value, (big_decimal){{10, 0, 0, 0, 0, 0}}, value);
  // Bset_scale(value, scale + 1);
  return err_code;
}

bool Bmantiss_div10(big_decimal *value) {
  // const uint8_t scale = Bget_scale(*value);
  const uint8_t err_code =
      Bs21_div(*value, (big_decimal){{10, 0, 0, 0, 0, 0}}, value);
  // Bset_scale(value, scale - 1);
  return err_code;
}

/* делится ли число нацело */
static bool is_Bmantiss_div10(big_decimal value) {
  big_decimal value_10 = (big_decimal){{10, 0, 0, 0, 0, 0}};
  Bset_sign(&value, plus);
  big_decimal Q = (big_decimal){{0}};  // частное quotient
  big_decimal R = (big_decimal){{0}};  // остаток remainder

  while (1) {
    for (u_int8_t i = 0; i < 6; ++i) { /* сбрасывает значение Q */
      Q.bits[i] = 0;
    }
    R = (big_decimal){{0}};

    for (int i = BMANTISS_BIT_COUNT - 1; i >= 0; i--) {
      Bleft_shift(&R);
      Bset_bit(&R, 0, Bget_bit(value, i));
      if (Bmantiss_ge(R, value_10)) {
        (void)Bmantiss_sub(R, value_10, &R);
        Bset_bit(&Q, i, 1);
      }
    }
    if (Bmantiss_ne(R, (big_decimal){{0}})) {
      return 1;
    } else {
      break;
    }
  }

  return 0;
}

void Bcircumcision(big_decimal *value) {
  int8_t mant_size = Bget_scale(*value);

  while (mant_size > 0 && is_Bmantiss_div10(*value) == 0) {
    Bmantiss_div10(value);
    mant_size--;
  }
  Bset_scale(value, mant_size);
}

void Balignment(big_decimal *value_1, big_decimal *value_2) {
  uint8_t mant_size1 = Bget_scale(*value_1);
  uint8_t mant_size2 = Bget_scale(*value_2);

  if (Bmantiss_eq(*value_1, *value_2)) {
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
    Bmantiss_mul10(value_1);
    mant_size1++;
  }

  // debug
  // {
  //   printf("\n1>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  //   Bdec_2bin(*value_1, 1, 1);
  //   printf("\n");
  //   Bdec_2bin(*value_2, 1, 1);
  //   printf("\n");
  //   printf("1<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");
  // }

  Bset_scale(value_1, mant_size1);
  while (mant_size1 < mant_size2) {
    if ((*value_2).bits[0] % 10 != 0) break;
    Bmantiss_div10(value_2);
    mant_size2--;
  }

  Bset_scale(value_2, mant_size2);
  while (mant_size1 < mant_size2) {
    Bmantiss_div10(value_2);
    mant_size2--;
  }
  Bset_scale(value_2, mant_size2);
}
