#include "rprintf.h"

#define VIDEO_MEMORY 0xB8000 //starting address
#define SCREEN_WIDTH 80 //colums
#define SCREEN_HEIGHT 25  //rows
#define COLOR 0x07   //light gray color

#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6 

const unsigned int multiboot_header[] __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12}; 


static int cursor_offset = 0;  //track where the next character will be printed


int get_execution(void) {

    return 0; //check execution level
}

//scroll the screen up and down
void scroll_screen(void) {
    volatile unsigned char *video = (volatile unsigned char *)VIDEO_MEMORY;
//copy each row except the first to the row above it
    for (int row = 0; row < SCREEN_HEIGHT - 1; row++) {
        for(int col= 0; col < SCREEN_WIDTH; col++) {
           int src_offset = (row + 1) * SCREEN_WIDTH * 2 + col * 2;
           int dst_offset = row * SCREEN_WIDTH * 2 +col * 2;
           video[dst_offset] = video[src_offset];  //copy character and byte from next to current row
           video[dst_offset + 1] = video [src_offset + 1];
        }
    }
//clear the last row after scrolling
    for(int col = 0; col < SCREEN_WIDTH; col++) {
       int offset = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2 + col * 2;
       video[offset] =' '; //space
       video[offset + 1] = COLOR; //default color
    }

    cursor_offset = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2; //reset to start of line
}
//write a single character to the screen at the cursor position
int putc(int data) {
    volatile unsigned char *video = (volatile unsigned char *)VIDEO_MEMORY;
    char c = (char)data;
//move to start of next time, beginning of current, and write to video memory
    if (c == '\n') {
        cursor_offset = (cursor_offset / (SCREEN_WIDTH * 2) + 1) * (SCREEN_WIDTH * 2);
    } else if (c== '\r') {
        cursor_offset = (cursor_offset / (SCREEN_WIDTH * 2)) * (SCREEN_WIDTH * 2);
    } else {
        video[cursor_offset] = c;
        video[cursor_offset + 1] = COLOR;
        cursor_offset += 2;
    }
//check if past screen size
    if (cursor_offset >= SCREEN_WIDTH * SCREEN_HEIGHT * 2) {
        scroll_screen();
    }
}
//clear the screen and print the level
void init_terminal(void) {
    volatile unsigned char *video = (volatile unsigned char *)VIDEO_MEMORY;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = COLOR;
    }
    cursor_offset = 0;

    esp_printf(putc, "Current execution: %d\r\n" , get_execution());
}

//entry point and initilize terminal to print
void main() {
    init_terminal();

    for (int i = 0; i < 2048; i++) {
        putc('c');
        putc('z');
        for (int j = 0; j < 204800; j++) {
        }
    }

    while(1) {
    }
}
