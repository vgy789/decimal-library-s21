#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <stdbool.h>
#include <stdint.h>

#include "big_decimal/big_decimal.h"

#define MAX_SCALE (28)
#define MAGNITUDE_BIT_COUNT (96)

/**
 * Структура для представления десятичного значения.
 *
 * bits[0], bits[1], и bits[2] содержат младшие, средние и старшие 32 бита
 * 96-разрядного целого числа. bits[3] содержит коэффициент масштабирования и
 * знак.
 *
 * @field bits Массив из четырех целых чисел, представляющих десятичное
 * значение.
 */
typedef struct {
  int bits[4];
} s21_decimal;

void alignment(s21_decimal *value1, s21_decimal *value2, bool for_add);

/**
 * Возвращает значение scale в s21_decimal.
 *
 * @param value s21_decimal.
 * @return значение scale.
 */
scale_t get_scale(s21_decimal value);

/**
 * Устанавливает scale для s21_decimal.
 *
 * @param value Указатель на изменяемый s21_decimal.
 * @param scale Новый масштаб для s21_decimal.
 * @return false если операция прошла успешно, true в противном случае.
 */
bool set_scale(s21_decimal *value, scale_t scale);

/**
 * Возвращает значение указанного бита в s21_decimal. Отсчёт с нуля.
 *
 * @param value s21_decimal.
 * @param bit_pos Позиция бита, значение которого нужно получить.
 * @return true если бит равен 1, false в противном случае.
 */
bool get_bit(s21_decimal value, uint8_t bit_pos);

/**
 * Устанавливает указанный бит числа s21_decimal. Отсчёт с нуля.
 *
 * @param value Указатель на изменяемый s21_decimal.
 * @param bit_pos Позиция бита, который нужно установить.
 * @param state Новое значение бита (true для 1, false для 0).
 */
void set_bit(s21_decimal *value, uint8_t bit_pos, bool state);

err_t big_to_decimal(big_decimal value, s21_decimal *result);

int big_to_int(big_decimal value);

void circumcision(s21_decimal *value);

err_t check_scale(s21_decimal value);

void decimal_to_big(s21_decimal value, big_decimal *result);

/**
 * Возварщает знак s21_decimal.
 *
 * @param value Проверяемое число.
 * @return 0 — положительный, 1 — отрицательный.
 */
bool get_sign(s21_decimal value);

/**
 * Устанавливает знак числа decimal.
 *
 * @param *value Указатель на изменяемое число.
 * @param sign Знак числа (0 — положительный, 1 — отрицательный).
 */
void set_sign(s21_decimal *value, bool sign);

err_t modulus10(s21_decimal value, s21_decimal *result);

err_t divide10(s21_decimal value, s21_decimal *result);

/**
 * Складывает два десятичных значения.
 *
 * Оператор '+'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @param result Указатель на результат сложения.
 * @return Код ошибки (0 - ок, 1 - число слишком велико или равно бесконечности,
 *         2 - число слишком мало или равно отрицательной бесконечности).
 */
err_t s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * Вычитает одно десятичное значение из другого.
 *
 * Оператор '-'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @param result Указатель на результат вычитания.
 * @return Код ошибки (0 - ок, 1 - число слишком велико или равно бесконечности,
 *         2 - число слишком мало или равно отрицательной бесконечности).
 */
err_t s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * Умножает два десятичных значения.
 *
 * Оператор '*'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @param result Указатель на результат умножения.
 * @return Код ошибки (0 - ок, 1 - число слишком велико или равно бесконечности,
 *         2 - число слишком мало или равно отрицательной бесконечности).
 */
err_t s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * Делит одно десятичное значение на другое.
 *
 * Оператор '/'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @param result Указатель на результат деления.
 * @return Код ошибки (0 - ок, 1 - число слишком велико или равно бесконечности,
 *         2 - число слишком мало или равно отрицательной бесконечности,
 *         3 - деление на 0).
 */
err_t s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * Проверяет, равны ли два десятичных значения.
 *
 * Оператор '=='
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 равно value_2, иначе 0.
 */
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);

/**
 * Проверяет, не равны ли два десятичных значения.
 *
 * Оператор '!='
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 не равно value_2, иначе 0.
 */
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение больше второго.
 *
 * Оператор '>'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 больше value_2, иначе 0.
 */
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение больше или равно второму.
 *
 * Оператор '>='
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 больше или равно value_2, иначе 0.
 */
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение меньше второго.
 *
 * Оператор '<'
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 меньше value_2, иначе 0.
 */
int s21_is_less(s21_decimal value_1, s21_decimal value_2);

/**
 * Проверяет, является ли первое десятичное значение меньше или равно второму.
 *
 * Оператор '<='
 *
 * @param value_1 Первое десятичное значение.
 * @param value_2 Второе десятичное значение.
 * @return 1 если value_1 меньше или равно value_2, иначе 0.
 */
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);

/**
 * Преобразует десятичное значение в целое число.
 *
 * @param src Исходное десятичное значение.
 * @param dst Указатель на результат преобразования.
 * @return Код ошибки (0 - ок, 1 - ошибка конвертации).
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst);

/**
 * Преобразует десятичное значение в число с плавающей запятой.
 *
 * @param src Исходное десятичное значение.
 * @param dst Указатель на результат преобразования.
 * @return Код ошибки (0 - ок, 1 - ошибка конвертации).
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst);

/**
 * Преобразует целое число в десятичное значение.
 *
 * @param src Исходное целое число.
 * @param dst Указатель на результат преобразования.
 * @return Код ошибки (0 - ок, 1 - ошибка конвертации).
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst);

/**
 * Преобразует число с плавающей запятой в десятичное значение.
 *
 * @param src Исходное число с плавающей запятой.
 * @param dst Указатель на результат преобразования.
 * @return Код ошибки (0 - ок, 1 - ошибка конвертации).
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst);

/**
 * Изменяет знак десятичного значения на противоположный.
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат изменения знака.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
err_t s21_negate(s21_decimal value, s21_decimal *result);

/**
 * Округляет десятичное значение до ближайшего целого числа в меньшую сторону.
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат округления.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
err_t s21_floor(s21_decimal value, s21_decimal *result);

/**
 * Округляет десятичное значение до ближайшего целого числа.
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат округления.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
err_t s21_round(s21_decimal value, s21_decimal *result);

/**
 * Усекание десятичного значения до целого числа.
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат усечения.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
err_t s21_truncate(s21_decimal value, s21_decimal *result);

/**
 * Банковское округление (в сторону четного числа).
 *
 * @param value Исходное десятичное значение.
 * @param result Указатель на результат усечения.
 * @return Код ошибки (0 - ок, 1 - ошибка вычисления).
 */
err_t bank_round(s21_decimal value, s21_decimal *result);

#endif  // S21_DECIMAL_H
