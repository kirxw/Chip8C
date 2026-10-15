// supposedly this prevents multi declarations, kind of cool thanks person on stackoverflow
#ifndef chip8_headers
#define chip8_headers

#include <stdint.h> // i don't THINK this will result in multiple inclusions?

void step_execution_loop(void);
void load_file(FILE *file_pointer);
void loop(void);

#endif