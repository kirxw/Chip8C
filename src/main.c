#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "chip8.h"
#include "render_loop.h"

int main(void) {
    FILE *file_pointer = fopen("roms/Blinky [Hans Christian Egeberg, 1991].ch8","r");
    load_file(file_pointer);
    fclose(file_pointer);
    return main_loop();
}