#pragma once
#include "libc.h"
#include <stdarg.h>


#define VIDEO_MEMORY ((uint8_t*)0xB8000)
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F
#define NULL ((void*)0)
//functionalities 


void puts(const char* str);
void putchar(char c);
void print_int(int num);
void print_uint(unsigned int num);
void print_hex(unsigned int num);
void clear_screen(void);
void set_cursor(uint16_t row, uint16_t col);
void put_newline(void);
void printf(const char* format, ...);
