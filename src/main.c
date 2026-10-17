#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "chip8.h"
#include "render_loop.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || (strcmp(argv[1],"-h")==0) || (strcmp(argv[1],"--help")==0) || (strcmp(argv[1],"help")==0)) {
        printf("\n\nAva's Chip8C\nSyntax: ./Chip8C <ROM_PATH>\n\nOptions:\n-c --cosmac\t\tEnable COSMAC behavior\n-a --amiga\t\tEnable AMIGA behavior\n-h --help help\t\tShow this message\n\n");
        return 0;
    }

    for (int i = 2; i < argc; i++) {
        char *arg = argv[i];
        if ((strcmp(arg,"-c") == 0) || (strcmp(arg,"--cosmac") == 0)) {
            COSMAC_MODE = 1;
        } else if ((strcmp(arg,"-a") == 0) || (strcmp(arg,"--amiga") == 0)) {
            AMIGA_MODE = 1;
        }
    }

    FILE *file_pointer = fopen(argv[1],"r");
    if (file_pointer == NULL) {
        printf("Failed to open file at '%s'",argv[1]);
        perror("");
        return 1;
    }

    load_file(file_pointer);
    fclose(file_pointer);
    return main_loop();
}