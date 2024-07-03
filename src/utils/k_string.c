#include "../include/k_string.h"

int k_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

void int_to_str(int num, char* str, int width) {
    int i = 0;
    int is_negative = 0;

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    while (i < width) {
        str[i++] = '0';
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - 1 - j];
        str[i - 1 - j] = temp;
    }
}

int k_sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int i = 0, j = 0;
    while (fmt[i]) {
        if (fmt[i] == '%' && fmt[i+1] == '0' && fmt[i+2] == '2' && fmt[i+3] == 'd') {
            int num = va_arg(args, int);
            char num_str[3];
            int_to_str(num, num_str, 2);
            buf[j++] = num_str[0];
            buf[j++] = num_str[1];
            i += 4;
        } else {
            buf[j++] = fmt[i++];
        }
    }

    buf[j] = '\0';
    va_end(args);
    return j;
}