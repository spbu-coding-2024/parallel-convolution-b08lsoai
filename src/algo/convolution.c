#include <stdio.h>
#include <stdlib.h>

#include "utils/filters.h"
#include "utils/image_io.h"

#define BOUND(value, min, max)                                                 \
  ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

static inline void process_pixel(int x, int y, int width, int height,
                                 int channels, unsigned char *image_data,
                                 unsigned char *new_image_data,
                                 double *filter_arr, int filter_size,
                                 int offset, double factor, double bias) {
  double red = 0.0, green = 0.0, blue = 0.0;
  for (int filter_y = 0; filter_y < filter_size; ++filter_y) {
    for (int filter_x = 0; filter_x < filter_size; ++filter_x) {
      int image_x = (x - offset + filter_x + width) % width;
      int image_y = (y - offset + filter_y + height) % height;
      int idx = (image_y * width + image_x) * channels;
      double filter_val = filter_arr[filter_y * filter_size + filter_x];

      red += image_data[idx] * filter_val;
      green += image_data[idx + 1] * filter_val;
      blue += image_data[idx + 2] * filter_val;
    }
  }

  int dst_idx = (y * width + x) * channels;

  double res_red = factor * red + bias;
  double res_green = factor * green + bias;
  double res_blue = factor * blue + bias;
  new_image_data[dst_idx] = (unsigned char)BOUND(res_red, 0, 255);
  new_image_data[dst_idx + 1] = (unsigned char)BOUND(res_green, 0, 255);
  new_image_data[dst_idx + 2] = (unsigned char)BOUND(res_blue, 0, 255);
}

image_t *seq_convolution(image_t *image, filter_t *filter) {
  if (image == NULL || filter == NULL) {
    fprintf(stderr, "Error: NULL pointer in convolution.\n");
    return NULL;
  }

  int height = image->height;
  int width = image->width;
  int channels = image->channels;
  unsigned char *image_data = image->image_data;

  double *filter_arr = filter->filter_arr;
  int filter_size = filter->size;

  image_t *new_image = create_empty_image(height, width, channels);
  if (!new_image) {
    fprintf(stderr, "Error: Cannot create new image.\n");
    return NULL;
  }
  unsigned char *new_image_data = new_image->image_data;

  int offset = filter_size / 2;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      process_pixel(x, y, width, height, channels, image_data, new_image_data,
                    filter_arr, filter_size, offset, filter->factor,
                    filter->bias);
    }
  }

  return new_image;
}
