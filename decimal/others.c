#include "others.h"
#include <math.h>

//void mul_by_ten(s21_decimal* value)
//{
// int buff = 0;
// int prev_buff = 0;
// int scale = get_scale(*value);
// for (int i = 0; i <= 2; i++)
// {
//  if (value->bits[i] > 0xFFFFFFF / 10)
//  {
//   buff = (value->bits[i] & (1 << 31)) >> 31;
//   value->bits[i + 1] += buff;
//  }
//  value->bits[i] = (value->bits[i] << 3) + (value->bits[i] << 1) + buff;
// }
// set_scale(value, ++scale);
//}

void mul_by_ten(s21_decimal* value) {
 int buff = 0;
 int zer_buff = 0;
 int scale = get_scale(*value);
 for (int i = 0; i <= 2; i++) {
  if (value->bits[i] > 0xFFFFFFF/10)
  { 
   buff = get_first_num(value->bits[i], &zer_buff);
   value->bits[i] -= buff * pow(10, zer_buff);
   value->bits[i] *= 10;
  }
  else
  {
   value->bits[i] = value->bits[i] * 10 + buff;
   buff = 0;
  }
 }
 set_scale(value, ++scale);
}

void div_by_ten(s21_decimal* value) {
 int buff[3];
 int scale = get_scale(*value);
 for (int i = 2; i >= 0; i--) {
  buff[i] = value->bits[i] % 10;
  value->bits[i] /= 10;
 }
 for (int i = 2; i >= 1; i--) {
  
  value->bits[i-1] += buff[i] * pow(10, 8);
 }
 set_scale(value, ++scale);
}

int get_scale(s21_decimal value) {
 return (value.bits[3] & (0b11111111 << 16)) >> 16;
}


bool set_scale(s21_decimal* value, int scale) {
 if (scale > 28) return 0;
 value->bits[3] = (0b11111111 << 16) & scale << 16;
}

int get_first_num(int value, int* size)
{
 int rez = 0;
 *size = 0;
 while (value > 0) {
  rez = value % 10;
  value /= 10;
  (*size)++;
 }
 (*size) -= 1;
 return rez;
}

void alignment(s21_decimal* value1, s21_decimal* value2) {
 int mant_size1 = get_scale(*value1), mant_size2 = get_scale(*value2);
 if (mant_size1 == mant_size2) return;
 if (mant_size2 < mant_size1) {
  alignment(value2, value1);
  return;
 }
 while(mant_size1 < mant_size2) {
  if ((*value1).bits[2] > 0xFFFFFFF / 10) break;
  mul_by_ten(value1);
  mant_size1++;
 }

 set_scale(value1, mant_size1);
 while (mant_size1 < mant_size2)
 {
  if ((*value2).bits[0] % 10 != 0) break;
  div_by_ten(value2);
  mant_size2--;
 }

 set_scale(value2, mant_size2);
 while (mant_size1 < mant_size2)
 {
  div_by_ten(value2);
  mant_size2--;
 }
 set_scale(value2, mant_size2);
}