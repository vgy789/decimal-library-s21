#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../big_decimal/big_decimal.h"
#include "../s21_decimal.h"

int big_to_int(big_decimal value) {
  s21_decimal dec_result = (s21_decimal){{0}};
  big_to_decimal(value, &dec_result);
  int result = 0;
  s21_from_decimal_to_int(dec_result, &result);
  return result;
}

err_t big_to_decimal(big_decimal value, s21_decimal *result) {
  *result = (s21_decimal){{0}};
  for (uint8_t i = 3; i < 6; ++i) {
    if (value.bits[i] != 0) { /* is overflow? */
      if (Bget_sign(value) == plus) {
        return 1;
      } else {
        return 2;
      }
    }
  }

  for (uint8_t i = 0; i < 3; ++i) {
    result->bits[i] = value.bits[i];
  }
  result->bits[3] = value.bits[6];

  return 0;
}

void decimal_to_big(s21_decimal value, big_decimal *result) {
  *result = (big_decimal){{0}};
  /* converter */
  for (uint8_t i = 0; i < 3; ++i) {
    result->bits[i] = value.bits[i];
  }
  result->bits[6] = value.bits[3];
}

err_t s21_from_decimal_to_int(s21_decimal src, int *dst) {
  const bool sign = get_sign(src);
  s21_truncate(src, &src);

  if (src.bits[1] != 0 || src.bits[2] != 0) { /* is overflow? */
    return 1;
  }

  *dst = src.bits[0];
  if (sign == minus) {
    *dst = -*dst;
  }

  return 0;
}

err_t s21_from_int_to_decimal(int src, s21_decimal *dst) {
  *dst = (s21_decimal){{0}};
  if (src < 0) { /* is negative? */
    src = -src;
    set_sign(dst, minus);
  }
  dst->bits[0] = src;

  return 0;
}

err_t s21_from_decimal_to_float(s21_decimal src, float *dst) {
  *dst = 0;
  const bool sign = get_sign(src);
  if (sign == minus) {
    *dst = -(*dst);
  }

  s21_decimal buf = (s21_decimal){{0}};
  scale_t scale = get_scale(src);

  char str_num[33] = {'\0'};
  int digit = 0;
  uint8_t i = 0;

  do {
    modulus10(src, &buf);
    divide10(src, &src);

    s21_from_decimal_to_int(buf, &digit);
    str_num[i] = (char)(digit + '0');
    i += 1;
    scale -= 1;
    if (scale == 0) {
      str_num[i] = '.';
      i += 1;
    }
  } while (s21_is_not_equal(src, (s21_decimal){{0}}));

  s21_strrev(str_num);

  bool is_first_zero = 0;
  bool has_point = false;   /* найдена . */
  bool significant = false; /* пройдено 7 значимых знаков */
  for (int i = 0; str_num[i] != '\0'; ++i) {
    if (str_num[i] == '.') {
      has_point = true;
      continue;
    }
    if (significant) {
      str_num[i] = '0';
      continue;
    }
    if (i + 1 - has_point - is_first_zero == 8) {
      // отсеиваем 8 значимых чисел, т.к. нужно будет округлить седьмую цифру по
      // значению восьмой
      significant = true;
      const uint8_t prev_value = str_num[i - 1] - '0';
      const uint8_t current_value = str_num[i] - '0';
      str_num[i - 1] =
          (roundf((float)(prev_value * 10 + current_value) / 10.f)) + '0';
      str_num[i] = '0';
    }
  }

  char *err;
  *dst = strtof(str_num, &err);
  if (sign == minus) {
    *dst = -*dst;
  }

  return 0;
}

err_t s21_from_float_to_decimal(float src, s21_decimal *dst) {
  const bool sign = signbit(src);
  src = fabs(src);
  if (isinf(src) || src > 79228162514264337593543950335.f ||
      (0 < src && src < 1e-28) || isnan(src)) {
    *dst = (s21_decimal){{0}};
    return 1;
  }

  char str_num[50] = {'\0'};
  sprintf(str_num, "%.7f", src);
  big_decimal Bdst = (big_decimal){{0}};
  big_decimal Bdst_for_test = (big_decimal){{0}};
  bool has_point = false;
  scale_t scale = 0;

  bool first_zero_check = false;
  bool significant = false;
  bool is_first_zero = 0;
  bool make_round = false;
  for (int i = 0; str_num[i] != '\0'; ++i) {
    if ((str_num[i] == '.')) {
      has_point = true;
      continue;
    }
    if (significant) {
      str_num[i] = '0';
    }

    if (first_zero_check == false && str_num[i] == '0') {
      /* первый нуль в целой части не является значимым 0.0456 */
      is_first_zero = true;
      first_zero_check = true;
      continue;
    }
    first_zero_check = true;
    if (has_point) {
      scale += 1;
    }

    if (i + 1 - has_point - is_first_zero == 7) {
      /* не больше 7 значимых цифр */
      significant = true;
      make_round = true;
    }
    Bdigits_mul10(&Bdst);
    Bdigits_mul10(&Bdst_for_test);
    const uint8_t curr_value = str_num[i] - '0';
    Bdigits_add(Bdst_for_test, (big_decimal){{curr_value}}, &Bdst_for_test);

    if (!make_round) {
      Bdigits_add(Bdst, (big_decimal){{curr_value}}, &Bdst);
    } else {
      const uint8_t next_value = str_num[i + 1] - '0';
      if (str_num[i + 1] != '\0') {
        Bdigits_add(Bdst,
                    (big_decimal){
                        {roundf((float)(curr_value * 10 + next_value) / 10.f)}},
                    &Bdst);
      } else {
        Bdigits_add(Bdst, (big_decimal){{curr_value}}, &Bdst);
      }
    }
    make_round = false;
  }

  Bset_scale(&Bdst, scale);
  Bnormalize(&Bdst);
  Bset_sign(&Bdst, sign);

  big_to_decimal(Bdst, dst);
  s21_decimal tmp = (s21_decimal){{0}};
  return (bool)big_to_decimal(Bdst_for_test, &tmp);
}
