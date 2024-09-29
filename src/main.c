#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "png.h"
#include "types.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        puts("usage: cimage <path>");
        return 1;
    }

    char *path = argv[1];

    FILE *image = fopen(path, "rb");
    if (!image) {
        fprintf(stderr, "fopen(): Failed to open `%s` file", path);
        return 1;
    }

    if (PNG_is_image(image) == 0) {
        printf("`%s` is an image!\n", path);
    } else {
        fprintf(stderr, "PNG_is_image(): File `%s` is not a valid PNG image", path);
        return 1;
    }

    // Ignore the signature bytes
    PNG_skip_signature(image);

    struct chunk_t chunk;
    while (strcmp(chunk.type, IEND) != 0) {
        if (PNG_read_chunk(&chunk, image) == 1) break;

        printf("%s:\n", chunk.type);

        if (strcmp(chunk.type, IHDR) == 0) {
            struct ihdr_chunk *ihdr = (struct ihdr_chunk *) chunk.data;
            printf("- width: %d\n- height: %d\n- bit depth: %d\n- color type: %d\n- compress method: %d\n- filter method: %d\n- interlace method: %d\n", htonl(ihdr->width), htonl(ihdr->height), ihdr->bit_depth, ihdr->color_type, ihdr->compress_method, ihdr->filter_method, ihdr->interlace_method);

        } else if (strcmp(chunk.type, gAMA) == 0) {
            dword gamma = *(dword *) chunk.data;
            printf("- gamma: %d\n", htonl(gamma));

        } else if (strcmp(chunk.type, pHYs) == 0) {
            struct phys_chunk *phys = (struct phys_chunk *) chunk.data;
            printf("- pixel per unit (x): %d\n- pixel per unit (y): %d\n- unit specifier: %d\n", htonl(phys->pixel_unit_x), htonl(phys->pixel_unit_y), phys->unit_specifier);

        } else if (strcmp(chunk.type, sRGB) == 0) {
            byte intent = *(byte *) chunk.data;
            printf("- rendering intent: %d\n", intent);
        }
    }

    return 0;
}