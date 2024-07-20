#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "decimal/bitwise_helper.h"
#include "decimal/s21_decimal.h"

int comparison_mantiss(s21_decimal a, s21_decimal b);

void print_bits(int num) {
    for(int i = 31; i >= 0; i --) {
        printf("%d", (num >> i) & 1); 
    }
    printf("\n");
}

void print_decimal(s21_decimal num) {
    for(int i = 3; i >=0; i--) {
        print_bits(num.bits[i]);
    }
    printf("\n");
}

int main() {
    s21_decimal a = {1, 0b11111111111111111111111111111111, 0b01111111111111111111111111111111, 0};
    s21_decimal b = {1, 0b11111111111111111111111111111111, 0b11111111111111111111111111111111, 0};
    int result = comparison_mantiss(a, b);
    printf("result = %d\n", result);
    print_decimal(a);
    print_decimal(b);

    return 0;
}

// int s21_is_less(s21_decimal a, s21_decimal b) {
//     //сперва узнаем положительные или отрицательные числа
    
//     //узнаем scale
//     //сравниваем scale
//     //если надо приводим к одному scale (домнажаем на 10)
//     //сравниваем мантиссы
// }

int comparison_mantiss(s21_decimal a, s21_decimal b){
    int bit_pos = 96;
    bool pos_a, pos_b;
    int result = 2;
    while(bit_pos >= 0) {
        pos_a = get_bit(a, bit_pos);
        pos_b = get_bit(b, bit_pos);
        if(pos_a > pos_b) {
            result = 1;
            break;
        }
        if(pos_a < pos_b) {
            result = 0;
            break;
        } 
        if(pos_a == pos_b) bit_pos--;
    }
    return result; // 2 -> a==b, 1 -> a > b, 0 -> a < b
}