#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "chip8.h"

int main(void) {
    FILE *file_pointer = fopen("roms/BC_test.ch8","r");
    load_file(file_pointer);
    fclose(file_pointer);
    loop();
    return 1;
}