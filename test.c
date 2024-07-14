#include <stdio.h>

typedef struct {
    int bits[4];
} s21_decimal;

// & и (умножение)
// | или (сложение)
// ^ исключающее или
// ~ инверсия/логический не

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

void set_sign(s21_decimal* num, int sign) {
    if(sign == 1) num->bits[3] |= 1 << 31;
    else {
        num->bits[3] &= ~(1 << 31);
    }
}

int main() {
    s21_decimal num = {1, 1 , 1 , 0}; // s21_decimal num = {1, 1 , 1 , -2147483648} => sign = 1;
    int sign = 1;
    print_decimal(num);
    set_sign(&num, sign);
    print_decimal(num);
    printf("%d", num.bits[3]);
    return 0;
}

