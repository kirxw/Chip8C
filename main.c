#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "chip8.h"

void main(void) {
    FILE *file_pointer = fopen("IBM Logo.ch8","r");
    load_file(file_pointer);
    fclose(file_pointer);
    loop();
}