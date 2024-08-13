#ifndef BIG_DECIMAL_H
#define BIG_DECIMAL_H

#include <stdbool.h>
#include <stdint.h>

#define BDIGITS_BIT_COUNT (192)

/**
 * Структура для представления десятичного значения.
 *
 * 192-разрядное число. bits[6] содержит коэффициент масштабирования и знак.
 *
 * @field bits Массив из семи целых чисел, представляющих десятичное значение.
 */
typedef struct {
  int bits[7];
} big_decimal;

typedef int8_t scale_t;
typedef int err_t;

/**
 * Знак числа.
 *
 * plus → 0, minus → 1.
 */
enum sign { plus, minus };

// divide деление, whole целое от деления, reside остаток от деления
enum { divide, whole, reside };

void Bnormalize(big_decimal *value);

void Bfix_bank_overflow(big_decimal *value);

/**
 * Вычитает из числа big_decimal единицу.
 */
err_t Bdecriment(big_decimal value, big_decimal *result);

/**
 * Прибавляет к числу big_decimal единицу.
 */
err_t Bincrement(big_decimal value, big_decimal *result);

/**
 * Делит на 10, отсекая разряд.
 */
err_t Bdivide10(big_decimal value, big_decimal *result);

err_t Bmodulus10(big_decimal value, big_decimal *result);

err_t Bdigits_div10(big_decimal *value);

err_t Bdigits_mul10(big_decimal *value);

int Bdigits_eq(big_decimal value_1, big_decimal value_2);

int Bdigits_ne(big_decimal value_1, big_decimal value_2);

int Bdigits_ge(big_decimal value_1, big_decimal value_2);

int Bdigits_gt(big_decimal value_1, big_decimal value_2);

int Bdigits_le(big_decimal value_1, big_decimal value_2);

int Bdigits_lt(big_decimal value_1, big_decimal value_2);

int Bmod(big_decimal value_1, big_decimal value_2);

/**
 * Складывает два десятичных значения.
 *
 * Оператор '+'
 */
err_t Bdigits_add(big_decimal value_1, big_decimal value_2,
                  big_decimal *result);

/**
 * Вычитает одно десятичное значение из другого.
 *
 * Оператор '-'
 */
err_t Bdigits_sub(big_decimal value_1, big_decimal value_2,
                  big_decimal *result);

/**
 * Возвращает указанный бит числа big_decimal. Отсчёт с нуля.
 */
bool Bget_bit(big_decimal value, uint8_t bit_pos);

/**
 * Устанавливает указанный бит числа big_decimal. Отсчёт с нуля.
 */
void Bset_bit(big_decimal *value, uint8_t bit_pos, bool status);

/**
 * Возвращает десятичную точку числа big_decimal.
 */
uint8_t Bget_scale(big_decimal value);

/**
 * Устанавливает десятичную точку в числе big_decimal.
 */
void Bset_scale(big_decimal *value, scale_t scale);

/**
 * Возварщает знак big_decimal.
 */
bool Bget_sign(big_decimal value);

/**
 * Устанавливает знак числа decimal.
 */
void Bset_sign(big_decimal *value, bool sign);

void Balignment(big_decimal *value_1, big_decimal *value_2, bool for_add);

/**
 * Перевод мантиссы в дополнительный код (дополнение до двух).
 */
void Bcompliment2(big_decimal value, big_decimal *result);

/**
 * Сдвиг битов мантиссы влево на один.
 */
err_t Bleft_shift(big_decimal *value);

/**
 * Сдвиг битов мантиссы враво на один.
 */
err_t Bright_shift(big_decimal *value);

void Bset_result_sign(big_decimal *value, bool sign);

/**
 * Умножает два десятичных значения.
 *
 * Оператор '*'
 */
err_t Bdigits_mul(big_decimal value_1, big_decimal value_2,
                  big_decimal *result);

err_t Bdigits_division(big_decimal value_1, big_decimal value_2,
                       big_decimal *result, uint8_t mode);

/**
 * Делит одно десятичное значение на другое.
 *
 * Оператор '/'
 */
err_t Bdigits_div(big_decimal value_1, big_decimal value_2,
                  big_decimal *result);

/**
 * Проверяет, равны ли два десятичных значения.
 *
 * Оператор '=='
 */
int Bs21_is_equal(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, не равны ли два десятичных значения.
 *
 * Оператор '!='
 */
int Bs21_is_not_equal(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение больше второго.
 *
 * Оператор '>'
 */
int Bs21_is_greater(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение больше или равно
 второму.
 *
 * Оператор '>='
 */
int Bs21_is_greater_or_equal(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение меньше второго.
 *
 * Оператор '<'
 */
int Bs21_is_less(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение меньше или равно
 второму.
 *
 * Оператор '<='
 */
int Bs21_is_less_or_equal(big_decimal value_1, big_decimal value_2);

/**
 * Преобразует десятичное значение в целое число.
 */
int Bs21_from_decimal_to_int(big_decimal src, int *dst);

/**
 * Преобразует десятичное значение в число с плавающей запятой.
 */
int Bs21_from_decimal_to_float(big_decimal src, float *dst);

/**
 * Преобразует целое число в десятичное значение.
 *
 */
int Bs21_from_int_to_decimal(int src, big_decimal *dst);

/**
 * Преобразует число с плавающей запятой в десятичное значение.
 */
int Bs21_from_float_to_decimal(float src, big_decimal *dst);

/**
 * Изменяет знак десятичного значения на противоположный.
 */
int Bs21_negate(big_decimal value, big_decimal *result);

/**
 * Округляет десятичное значение до ближайшего целого числа в меньшую
 сторону.
 */
void Bs21_floor(big_decimal value, big_decimal *result);

/**
 * Округляет десятичное значение до ближайшего целого числа.
 */
void Bs21_round(big_decimal value, big_decimal *result);

/**
 * Усекание десятичного значения до целого числа.
 */
void Bs21_truncate(big_decimal value, big_decimal *result);

/**
 * Банковское округление (в сторону четного числа).
 */
void Bbank_round(big_decimal value, big_decimal *result);

#endif  // BIG_DECIMAL_H
