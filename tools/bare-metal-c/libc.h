#pragma once

// as i am trying not to include stdint and stddef
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef uint32_t           size_t;
typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;


// Function declarations
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
size_t strlen(const char* str); //done
int strcmp(const char* s1, const char* s2); //done
char* strcpy(char* dest, const char* src);//done
char* strncpy(char* dest, const char* src, size_t n);//done
char* strcat(char* dest, const char* src);//done
int atoi(const char* str);
void itoa(int value, char* str, int base);
