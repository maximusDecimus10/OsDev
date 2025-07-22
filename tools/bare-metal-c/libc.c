#include "libc.h"

size_t strlen(const char* str){
    size_t retval = 0;
    while(str[retval] != '\0') retval++;
    return retval;
}

int strcmp(const char* s1, const char* s2) {
    size_t i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return 1;
        }
        i++;
    }
    if (s1[i] != s2[i]) {
        return 1;
    }
    return 0;
}

char* strcpy(char* dest, const char* src){
    int i = 0;
    while(src[i]!='\0'){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

char* strncpy(char* dest, const char* src, unsigned int n) {
    unsigned int i = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}
char* strcat(char* dest, const char* src) {
    unsigned int i = 0;
    unsigned int j = 0;
    while (dest[i] != '\0') {
        i++;
    }
    while (src[j] != '\0') {
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = '\0';

    return dest;
}

int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    int i = 0;
    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return sign * result;
}
void itoa(int value, char* str) {
    int i = 0;
    int is_negative = 0;
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }
    while (value != 0) {
        int digit = value % 10;
        str[i++] = '0' + digit;
        value /= 10;
    }
    if (is_negative) {
        str[i++] = '-';
    }
    str[i] = '\0';
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}
void* memcpy(void* dest, const void* src, uint32_t n){
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for(uint32_t i = 0; i<n; i++)
        d[i] = s[i];
    return dest;
}
void* memset(void* ptr, int value, uint32_t n){
    uint8_t* p = (uint8_t*) ptr;
    for(uint32_t i = 0; i<n; i++)
        p[i] = (uint8_t)value;
    return ptr;
}
