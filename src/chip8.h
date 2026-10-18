// supposedly this prevents multi declarations, kind of cool thanks person on stackoverflow
#ifndef chip8_headers
#define chip8_headers

#include <stdint.h> // i don't THINK this will result in multiple inclusions?
#include <stdio.h>

extern uint64_t display[32];
extern uint16_t keyboard;
extern uint8_t sound_timer;
extern uint8_t delay_timer;

extern int SHOULD_RENDER;
extern int COSMAC_MODE;
extern int AMIGA_MODE;

extern int drew_this_frame;

void step_chip(void);
void load_file(FILE *file_pointer);

#endif