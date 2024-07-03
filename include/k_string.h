// include/k_string.h

#ifndef K_STRING_H
#define K_STRING_H
#include <stdarg.h>

int k_strcmp(const char *str1, const char *str2);
int k_sprintf(char *buf, const char *fmt, ...);
void int_to_str(int num, char* str, int width);

#endif