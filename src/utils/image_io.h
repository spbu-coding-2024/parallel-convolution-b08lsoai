#pragma once

typedef struct {
  unsigned char *image_data;
  int height;
  int width;
  int channels;
} image_t;

image_t *load_image(const char *filename);
image_t *create_empty_image(int height, int width, int channels);
int store_image(const char *filename, image_t *image);
void free_image(image_t *image);
const char *generate_filename(const char *filename, const char *filter_name);
