
#include "rprintf.h"
#include <stdint.h>
#define VIDEO_MEMORY 0xB8000 //starting address
#define SCREEN_WIDTH 80 //colums
#define SCREEN_HEIGHT 25  //rows
#define COLOR 0x07   //light gray color

#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
#define KEYBOARD_PORT 0X60 

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

    
}

unsigned char keyboard_map[128] =
{
   0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
 '9', '0', '-', '=', '\b',     /* Backspace */
 '\t',                 /* Tab */
 'q', 'w', 'e', 'r',   /* 19 */
 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
   0,                  /* 29   - Control */
 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
'\'', '`',   0,                /* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
 'm', ',', '.', '/',   0,                              /* Right shift */
 '*',
   0,  /* Alt */
 ' ',  /* Space bar */
   0,  /* Caps lock */
   0,  /* 59 - F1 key ... > */
   0,   0,   0,   0,   0,   0,   0,   0,  
   0,  /* < ... F10 */
   0,  /* 69 - Num lock*/
   0,  /* Scroll Lock */
   0,  /* Home key */
   0,  /* Up Arrow */
   0,  /* Page Up */
 '-',
   0,  /* Left Arrow */
   0,  
   0,  /* Right Arrow */
 '+',
   0,  /* 79 - End key*/
   0,  /* Down Arrow */
   0,  /* Page Down */
   0,  /* Insert Key */
   0,  /* Delete Key */
   0,   0,   0,  
   0,  /* F11 Key */
   0,  /* F12 Key */
   0,  /* All other keys are undefined */
};

unsigned char read_scancode() {
    unsigned char scancode;
    __asm__ __volatile__("inb %1, %0" : "=a"(scancode) : "Nd"(KEYBOARD_PORT));
    return scancode;
}

unsigned char read_status() {
    unsigned char status;
    __asm__ __volatile__("inb %1, %0" : "=a"(status) : "Nd"(0x64));
    return status;
}

void keyboard_handler(unsigned char scancode) {
     static unsigned char last_scancode = 0;

    if(scancode & 0x80) {
       last_scancode = 0;
       return;
}
    if(scancode == last_scancode) {
       return;
}
unsigned char ch = keyboard_map[scancode];

   if(ch) {

     esp_printf(putc, "0x%02x %c\r\n", scancode, ch);
   } else {
       esp_printf(putc, "0x%02x [unknown]\r\n", scancode);
}
}


//entry point and initilize terminal to print
void main() {
    init_terminal();

    while(1) {
        if(read_status() & 1) {
 
        unsigned char scancode = read_scancode(); 

        keyboard_handler(scancode); 
}
}
}

