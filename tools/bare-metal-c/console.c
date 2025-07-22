#include "console.h"




static uint16_t cursor_row = 0;
static uint16_t cursor_col = 0;

void set_cursor(uint16_t row, uint16_t col) {
    cursor_row = row;
    cursor_col = col;
}
void put_newline(void) {
    cursor_row++;
    cursor_col = 0;
    if (cursor_row >= MAX_ROWS) {
        cursor_row = 0;
    }
}
void clear_screen(void) {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            int offset = 2 * (row * MAX_COLS + col);
            VIDEO_MEMORY[offset] = ' ';
            VIDEO_MEMORY[offset + 1] = WHITE_ON_BLACK;
        }
    }
    set_cursor(0, 0);
}

void putchar(char c) {
    if (c == '\n') {
        put_newline();
        return;
    }
    int offset = 2 * (cursor_row * MAX_COLS + cursor_col);
    VIDEO_MEMORY[offset] = c;
    VIDEO_MEMORY[offset + 1] = WHITE_ON_BLACK;

    cursor_col++;
    if (cursor_col >= MAX_COLS) {
        put_newline();
    }
}

void puts(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }
}

void print_uint(unsigned int num) {
    char buf[11];  
    int i = 0;
    if (num == 0) {
        putchar('0');
        return;
    }
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i--) {
        putchar(buf[i]);
    }
}

void print_int(int num) {
    if (num < 0) {
        putchar('-');
        print_uint(-num);
    } else {
        print_uint(num);
    }
}

void print_hex(unsigned int num) {
    char hex_chars[] = "0123456789ABCDEF";
    putchar('0');
    putchar('x');
    for (int i = 28; i >= 0; i -= 4) {
        putchar(hex_chars[(num >> i) & 0xF]);
    }
}

void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            char spec = format[i];
            switch (spec) {
                case 'd': print_int(va_arg(args, int)); break;
                case 'u': print_uint(va_arg(args, unsigned int)); break;
                case 'x': print_hex(va_arg(args, unsigned int)); break;
                case 'c': putchar((char)va_arg(args, int)); break;
                case 's': puts(va_arg(args, const char*)); break;
                case '%': putchar('%'); break;
                default: putchar('?'); break;
            }
        } else {
            putchar(format[i]);
        }
    }
    va_end(args);
}
