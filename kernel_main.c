#include "rprintf.h"

#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define COLOR 0x07


static int cursor_offset = 0;


int get_execution(void) {

    return 42;
}


void scroll_screen(void) {
    volatile unsigned char *video = (volatile unsigned char *)VIDEO_MEMORY;

    for (int row = 0; row < SCREEN_HEIGHT - 1; row++) {
        for(int col= 0; col < SCREEN_WIDTH; col++) {
           int src_offset = (row + 1) * SCREEN_WIDTH * 2 + col * 2;
           int dst_offset = row * SCREEN_WIDTH * 2 +col * 2;
           video[dst_offset] = video[src_offset];
           video[dst_offset + 1] = video [src_offset + 1];
        }
    }

    for(int col = 0; col < SCREEN_WIDTH; col++) {
       int offset = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2 + col * 2;
       video[offset] =' ';
       video[offset + 1] = COLOR;
    }

    cursor_offset = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2;
}

void putc(int data) {
    volatile unsigned char *video = (volatile unsigned char *)VIDEO_MEMORY;
    char c = (char)data;

    if (c == '\n') {
        cursor_offset = (cursor_offset / (SCREEN_WIDTH * 2) + 1) * (SCREEN_WIDTH * 2);
    } else if (c== '\r') {
        cursor_offset = (cursor_offset / (SCREEN_WIDTH * 2)) * (SCREEN_WIDTH * 2);
    } else {
        video[cursor_offset] = c;
        video[cursor_offset + 1] = COLOR;
        cursor_offset += 2;
    }

    if (cursor_offset >= SCREEN_WIDTH * SCREEN_HEIGHT * 2) {
        scroll_screen();
    }
}

void init_terminal(void) {
    volatile unsigned char *video = (volatile unsigned char *)VIDEO_MEMORY;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = COLOR;
    }
    cursor_offset = 0;

    esp_printf(putc, "current execution: %d\r\n ", get_execution());
}


void kernel_main(void) {
    init_terminal();
}
