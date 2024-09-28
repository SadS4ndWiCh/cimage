#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Critical Chunks
#define IHDR 0x49484452
#define PLTE 0x504c5445
#define IDAT 0x49444154
#define IEND 0x49454e44

// Ancillary Chunks
#define pHYs 0x70485973
#define sRGB 0x73524742
#define gAMA 0x67414d41

// Chunk Layout
struct chunk_t {
    uint32_t length; // 4 byte
    uint32_t type;   // 4 byte
    uint32_t crc;    // 4 byte
    uint8_t *data;   // 1 byte
};

// IHDR Chunk Data
struct ihdr_chunk {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compress_method;
    uint8_t filter_method;
    uint8_t interlace_method;
};

struct PNG {
    struct ihdr_chunk *ihdr;
};

struct chunk_t *read_chunk(FILE *fp) {
    size_t ret;
    struct chunk_t *chunk = (struct chunk_t *) malloc(sizeof(struct chunk_t));

    ret = fread(&chunk->length, sizeof(chunk->length), 1, fp);
    if (ret == 0) {
        fprintf(stderr, "[CHUNK_ERROR] Failed to read `length` chunk\n");
        return NULL;
    }
    chunk->length = htonl(chunk->length);

    ret = fread(&chunk->type, sizeof(chunk->type), 1, fp);
    if (ret == 0) {
        fprintf(stderr, "[CHUNK_ERROR] Failed to read `type` chunk\n");
        return NULL;
    }

    if (chunk->length > 0) {
        chunk->data = (uint8_t *) malloc(chunk->length);

        ret = fread(chunk->data, sizeof(uint8_t), chunk->length, fp);
        if (ret == 0) {
            fprintf(stderr, "[CHUNK_ERROR] Failed to read `data` chunk\n");
            return NULL;
        }
    }

    ret = fread(&chunk->crc, sizeof(chunk->crc), 1, fp);
    if (ret == 0) {
        fprintf(stderr, "[CHUNK_ERROR] Failed to read `CRC` chunk\n");
        return NULL;
    }
    chunk->crc = htonl(chunk->crc);

    return chunk;
}

void show_chunk(struct chunk_t *chunk) {
    printf("[%d]\nLength: %d\nCRC: %d\n", chunk->type, chunk->length, chunk->crc);
}

void free_chunk(struct chunk_t *chunk) {
    free(chunk->data);
    free(chunk);
}

int main(void) {
    FILE *fp = fopen("sample.png", "rb");
    if (!fp) {
        puts("[ERROR] Failed to open `sample.png` file");
        return 1;
    }

    unsigned char sig[8];
    size_t ret = fread(&sig, ARRAY_SIZE(sig), sizeof(*sig), fp);
    if (ret != sizeof(*sig)) {
        fprintf(stderr, "fread() failed: %zu\n", ret);
        return 1;
    }

    struct PNG image;

    struct chunk_t *chunk = read_chunk(fp);
    if (htonl(chunk->type) != IHDR) {
        fprintf(stderr, "read_chunk() first chunk must be IHDR: %d", chunk->type);
        return 1;
    }

    image.ihdr = (struct ihdr_chunk *) chunk->data;
    
    printf("IMAGE: %dx%d\n", htonl(image.ihdr->width), htonl(image.ihdr->height));

    free(chunk);
    free(image.ihdr);

    fclose(fp);
    return 0;
}