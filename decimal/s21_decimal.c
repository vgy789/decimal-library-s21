#include "s21_decimal.h"

#include "../bitwise_helper.h"

void set_sign(s21_decimal *dec, bool sign) {
  if (sign == plus) {
    dec->bits[3] &= ~(1 << 31);
  } else {
    dec->bits[3] |= (1 << 31);
  }
}

bool get_sign(s21_decimal dec) { return ((dec.bits[3] >> 31) & 1); }

void set_bit(s21_decimal *dec, size_t bit_pos, bool status) {
  char int_part = 0;

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  if (status == false) {
    dec->bits[int_part] &= ~(1 << bit_pos);
  } else {
    dec->bits[int_part] |= (1 << bit_pos);
  }
}

bool get_bit(s21_decimal dec, size_t bit_pos) {
  char int_part = 0;

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  return (dec.bits[int_part] & (1 << bit_pos));
};

void uint_binary(unsigned int value) {
  int bits = 31;

  // value = abs(value);
  while (value != 0) {
    int buf = value - (1 << bits);

    if (buf >= 0) {
      value = buf;
      printf("1");
    } else {
      printf("0");
    }
    --bits;
  }
  while (bits >= 0) {
    printf("0");
    --bits;
  }
}

void print_0b_decimal(s21_decimal value) {
  if (get_sign(value))
    printf("-");
  else
    printf("+");
  uint_binary(value.bits[2]);
  uint_binary(value.bits[1]);
  uint_binary(value.bits[0]);
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  const bool sign = get_sign(value);
  int bit_i = 0;

  while (bit_i < 96) {
    const bool bit = get_bit(value, bit_i);
    set_bit(result, bit_i, !bit);
    bit_i += 1;
  }
  int is_err = s21_add(*result, (s21_decimal){0b1}, result);
  set_sign(result, ~sign);
  // TODO: Возвращает ошибку. О какой ошибке идёт речь?
  return 0;
}

int s21_add(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  int bit_i = 0;
  bool bit_transfer = false;

  while (bit_i < 96) {
    const bool bit1 = get_bit(value1, bit_i);
    const bool bit2 = get_bit(value2, bit_i);
    bool bit_result = bit1 ^ bit2 ^ bit_transfer;
    set_bit(result, bit_i, bit_result);
    bit_transfer = (bit1 & bit2) | (bit1 & bit_transfer) | (bit2 & bit_transfer);

    bit_i += 1;
  }

  // TODO: переполнение, установка знака у результата при переполнениии. Ошибка при переполнении.


  // const bool err_overflow = bit_transfer == true && !(get_sign(value1) ^ get_sign(value2));
  // if (err_overflow) { /* test overflow */
  //   printf("ERROR\n");
  // }

  // TODO: ошибка
  return;
}

int s21_sub(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  s21_negate(value2, &value2);
  int is_err = s21_add(value1, value2, result);
  // TODO: ошибка
  return;
}

int s21_mul(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  int is_err = 0;
  *result = (s21_decimal){0b0};

  const bool sign1 = get_sign(value1);
  const bool sign2 = get_sign(value2);
  if (sign1 == minus)
    s21_negate(value2, &value1);
  if (sign2 == minus)
    s21_negate(value2, &value2);

  while(value2.bits[0] | value2.bits[1] | value2.bits[2]) {
    s21_sub(value2, (s21_decimal){0b1}, &value2);
    is_err = s21_add(value1, *result, result);
  }

  const bool sign_result = sign1 ^ sign2;
  if (sign_result == minus)
    s21_negate(*result, result);
  
  // TODO: ошибка
  return;
}

// int s21_div(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  // нужна операция сравнения >=
// }
