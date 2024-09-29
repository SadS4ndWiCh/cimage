#include <stdio.h>

#include "types.h"

#ifndef _PNGH_
#define _PNGH_

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Critical Chunks
#define IHDR "IHDR"
#define PLTE "PLTE"
#define IDAT "IDAT"
#define IEND "IEND"

// Ancillary Chunks
#define pHYs "pHYs"
#define sRGB "sRGB"
#define gAMA "gAMA"

/* 
    struct chunk_t - PNG File Chunks Layout.
*/
struct chunk_t {
  dword length; // The length of chunk's data in bytes.
  byte type[5]; // A four bytes chunk name.
  byte *data;   // The data of the chunk. Chunk can have no data.
  dword crc;    // A Cyclic Redundancy Check, present even in no data chunk.
};

/*
    struct ihdr_chunk - IHDR Chunk.
*/
struct ihdr_chunk {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compress_method;
    uint8_t filter_method;
    uint8_t interlace_method;
};

/*
    struct phys_chunk - pHYs Chunk.
*/
struct phys_chunk {
    uint32_t pixel_unit_x;
    uint32_t pixel_unit_y;
    uint8_t unit_specifier;
};

/*
    int PNG_read_chunk - Read a single chunk from image file.

    @param chunk The output chunk struct.
    @param image Image file pointer.

    @returns The number 0 if successfuly read the chunk or 1 otherwise.
*/
int PNG_read_chunk(struct chunk_t *chunk, FILE *image);

/*
    void PNG_skip_signature - Skip the signature bytes

    @param image The image file pointer.
*/
void PNG_skip_signature(FILE *image);

/*
    int PNG_is_image - Check if the file is an image

    @param image The image file pointer.

    @returns The number 0 if file is an image or 1 otherwise.
*/
int PNG_is_image(FILE *image);

#endif