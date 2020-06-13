#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum allocation_type {
    NO_ALLOCATION, SELF_ALLOCATED, STB_ALLOCATED
};

typedef struct {
    int width;
    int height;
    int channels;
    size_t size;
    uint8_t *data;
    enum allocation_type allocation_;
} Image;

void Image_load(Image *img, const char *fname);
void Image_create(Image *img, int width, int height, int channels, bool zeroed);
void Image_save(const Image *img, const char *fname);
void Image_free(Image *img);
void Image_to_gray(const Image *orig, Image *gray);
void Image_to_sepia(const Image *orig, Image *sepia);

void Image_to_binary(const Image *orig, Image *binary, const int umbral);
void Image_to_binary_from_gray(const Image *orig, Image *binary, const int umbral);
void Image_es_casiNegra(const Image *orig, const int umbral, const char *nombre);
void Image_lapleciano(const Image *orig, Image *lapleciano);