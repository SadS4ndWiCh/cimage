#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "png.h"

int PNG_read_chunk(struct chunk_t *chunk, FILE *image) {
    size_t ret;

    ret = fread(&chunk->length, sizeof(chunk->length), 1, image);
    if (ret == 0) {
        fprintf(stderr, "[CHUNK_ERROR] Failed to read `length` chunk\n");
        return 1;
    }
    chunk->length = htonl(chunk->length);

    ret = fread(&chunk->type, sizeof(*chunk->type), ARRAY_SIZE(chunk->type) - 1, image);
    if (ret == 0) {
        fprintf(stderr, "[CHUNK_ERROR] Failed to read `type` chunk\n");
        return 1;
    }

    if (chunk->length > 0) {
        chunk->data = (uint8_t *) malloc(chunk->length);

        ret = fread(chunk->data, sizeof(*chunk->data), chunk->length, image);
        if (ret == 0) {
            fprintf(stderr, "[CHUNK_ERROR] Failed to read `data` chunk\n");
            return 1;
        }
    }

    ret = fread(&chunk->crc, sizeof(chunk->crc), 1, image);
    if (ret == 0) {
        fprintf(stderr, "[CHUNK_ERROR] Failed to read `CRC` chunk\n");
        return 1;
    }
    chunk->crc = htonl(chunk->crc);

    return 0;
}

void PNG_skip_signature(FILE *image) {
    if (ftell(image) <= 8) fseek(image, 8L, SEEK_SET);
}

int PNG_is_image(FILE *image) {
    byte signature[8];

    // Save the current position and back to start of file
    long backpoint = ftell(image);
    fseek(image, 0L, SEEK_SET);
    
    // Read the signature bytes and go back to backpoint
    size_t ret = fread(&signature, sizeof(*signature), ARRAY_SIZE(signature), image);
    fseek(image, backpoint, SEEK_SET);

    if (
        ret != 0 &&
        signature[0] == 0x89 && signature[1] == 0x50 && signature[2] == 0x4e && signature[3] == 0x47 &&
        signature[4] == 0xd && signature[5] == 0xa && signature[6] == 0x1a && signature[7] == 0xa
    ) {
        return 0;
    }

    return 1;
}

