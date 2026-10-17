#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "chip8.h"
#include "render_loop.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || argv[1] == "-h" || argv[1] == "--help" || argv[1] == "help") {
        printf("\n\nAva's Chip8C\nSyntax: ./Chip8C <ROM_PATH>\n\nOptions:\n-c --cosmac\t\tEnable COSMAC behavior\n-a --amiga\t\tEnable AMIGA behavior\n\n");
        return 0;
    }

    FILE *file_pointer = fopen(argv[1],"r");
    if (file_pointer == NULL) {
        perror("Failed to open file\n");
        return 1;
    }

    load_file(file_pointer);
    fclose(file_pointer);
    return main_loop();
}