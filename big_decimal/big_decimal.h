#ifndef BIG_DECIMAL_H
#define BIG_DECIMAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define BMANTISS_BIT_COUNT (192)

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

/**
 * Знак числа.
 *
 * plus → 0, minus → 1.
 */
enum sign { plus, minus };

/**
 * Складывает два десятичных значения.
 *
 * Оператор '+'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @param result Указатель на результат сложения.
 * @return Код ошибки (0 - ок, 1 - число слишком велико или равно
 бесконечности,
 *         2 - число слишком мало или равно отрицательной бесконечности).
 */
int Bmantiss_add(big_decimal value_1, big_decimal value_2, big_decimal *result);

/**
 * Вычитает одно десятичное значение из другого.
 *
 * Оператор '-'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @param result Указатель на результат вычитания.
 * @return Код ошибки (0 - ок, 1 - число слишком велико или равно
 бесконечности,
 *         2 - число слишком мало или равно отрицательной бесконечности).
 */
int Bmantiss_sub(big_decimal value_1, big_decimal value_2, big_decimal *result);

/**
 * Умножает два десятичных значения.
 *
 * Оператор '*'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @param result Указатель на результат умножения.
 * @return Код ошибки (0 - ок, 1 - число слишком велико или равно
 бесконечности,
 *         2 - число слишком мало или равно отрицательной бесконечности).
 */
int Bs21_mul(big_decimal value_1, big_decimal value_2, big_decimal *result);

/**
 * Делит одно десятичное значение на другое.
 *
 * Оператор '/'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @param result Указатель на результат деления.
 * @return Код ошибки (0 - ок, 1 - число слишком велико или равно
 бесконечности,
 *         2 - число слишком мало или равно отрицательной бесконечности,
 *         3 - деление на 0).
 */
int Bs21_div(big_decimal value_1, big_decimal value_2, big_decimal *result);

/**
 * Вычитает из числа big_decimal единицу.
 */
int Bs21_dec(big_decimal value, big_decimal *result);

/**
 * Прибавляет к числу big_decimal единицу.
 */
int Bs21_inc(big_decimal value, big_decimal *result);

bool Bmantiss_div10(big_decimal *value);

bool Bmantiss_mul10(big_decimal *value);

int Bmantiss_eq(big_decimal value_1, big_decimal value_2);

int Bmantiss_ne(big_decimal value_1, big_decimal value_2);

int Bmantiss_ge(big_decimal value_1, big_decimal value_2);

int Bmantiss_gt(big_decimal value_1, big_decimal value_2);

int Bmantiss_le(big_decimal value_1, big_decimal value_2);

int Bmantiss_lt(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, равны ли два десятичных значения.
 *
 * Оператор '=='
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 равно value_2, иначе 0.
 */
int Bs21_is_equal(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, не равны ли два десятичных значения.
 *
 * Оператор '!='
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 не равно value_2, иначе 0.
 */
int Bs21_is_not_equal(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение больше второго.
 *
 * Оператор '>'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 больше value_2, иначе 0.
 */
int Bs21_is_greater(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение больше или равно
 второму.
 *
 * Оператор '>='
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 больше или равно value_2, иначе 0.
 */
int Bs21_is_greater_or_equal(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение меньше второго.
 *
 * Оператор '<'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 меньше value_2, иначе 0.
 */
int Bs21_is_less(big_decimal value_1, big_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение меньше или равно
 второму.
 *
 * Оператор '<='
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 меньше или равно value_2, иначе 0.
 */
int Bs21_is_less_or_equal(big_decimal value_1, big_decimal value_2);

/**
 * Преобразует десятичное значение в целое число.
 *
 * @param src Исходное десятичное значение.
 * @param dst Указатель на результат преобразования.
 * @return Код ошибки (0 - ок, 1 - ошибка конвертации).
 */
int Bs21_from_decimal_to_int(big_decimal src, int *dst);

/**
 * Преобразует десятичное значение в число с плавающей запятой.
 *
 * @param src Исходное десятичное значение.
 * @param dst Указатель на результат преобразования.
 * @return Код ошибки (0 - ок, 1 - ошибка конвертации).
 */
int Bs21_from_decimal_to_float(big_decimal src, float *dst);

/**
 * Преобразует целое число в десятичное значение.
 *
 * @param src Исходное целое число.
 * @param dst Указатель на результат преобразования.
 * @return Код ошибки (0 - ок, 1 - ошибка конвертации).
 */
int Bs21_from_int_to_decimal(int src, big_decimal *dst);

/**
 * Преобразует число с плавающей запятой в десятичное значение.
 *
 * @param src Исходное число с плавающей запятой.
 * @param dst Указатель на результат преобразования.
 * @return Код ошибки (0 - ок, 1 - ошибка конвертации).
 */
int Bs21_from_float_to_decimal(float src, big_decimal *dst);

/**
 * Изменяет знак десятичного значения на противоположный.
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат изменения знака.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
int Bs21_negate(big_decimal value, big_decimal *result);

/**
 * Округляет десятичное значение до ближайшего целого числа в меньшую
 сторону.
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат округления.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
int Bs21_floor(big_decimal value, big_decimal *result);

/**
 * Округляет десятичное значение до ближайшего целого числа.
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат округления.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
int Bs21_round(big_decimal value, big_decimal *result);

/**
 * Усекание десятичного значения до целого числа.
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат усечения.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
int Bs21_truncate(big_decimal value, big_decimal *result);

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
void Bset_scale(big_decimal *value, uint8_t scale);

/**
 * Возварщает знак big_decimal.
 *
 * @param value Проверяемое число.
 * @return 0 — положительный, 1 — отрицательный.
 */
bool Bget_sign(big_decimal value);

/**
 * Устанавливает знак числа decimal.
 *
 * @param *value Указатель на изменяемое число.
 * @param sign Знак числа (0 — положительный, 1 — отрицательный).
 */
void Bset_sign(big_decimal *value, bool sign);

void Balignment(big_decimal *value1, big_decimal *value2);

void Bcircumcision(big_decimal *value);

/**
 * Перевод мантиссы в дополнительный код (дополнение до двух).
 */
void Bcompliment2(big_decimal value, big_decimal *result);

/**
 * Сдвиг битов мантиссы влево на один.
 */
void Bleft_shift(big_decimal *value);

int Bs21_div2(big_decimal value_1, big_decimal value_2, big_decimal *result);

#endif  // BIG_DECIMAL_H
