#ifndef OTHERS.H
#define OTHERS .H

#include "s21_decimal.h"

typedef struct s21_decimal {
  int bits[4];
} s21_decimal;

void mul_by_ten(s21_decimal* value);
void div_by_ten(s21_decimal* value);

bool set_scale(s21_decimal* value, int scale);
int get_scale(s21_decimal value);

int get_first_num(int value, int* size);

void alignment(s21_decimal* value1, s21_decimal* value2);  //выравнивает мантисы
#endif OTHERS.H