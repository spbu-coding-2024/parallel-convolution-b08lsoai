#include <stdio.h>

#include "convolution.h"

#define FREE_IMAGES                                                            \
  free_image(image);                                                           \
  free_image(new_image)

int process_image(args_t *args) {
  if (!args) {
    fprintf(stderr, "Error: args are NULL\n");
    return -1;
  }

  filter_t *filter = create_filter(args->filter);
  if (!filter) {
    fprintf(stderr, "Error: failed to create filter\n");
    return -1;
  }

  const char *filter_name = filter->name;
  conv_mode mode = args->mode;

  for (size_t i = 0; i < args->images_number; i++) {
    const char *filename = args->filenames[i];
    image_t *image = NULL;
    image_t *new_image = NULL;

    image = load_image(filename);
    if (!image) {
      free_filter(filter);
      return -1;
    }

    switch (mode) {
    case MODE_SEQ:
      new_image = seq_convolution(image, filter);
      break;

    default:
      fprintf(stderr, "Error: unknown mode %d\n", mode);
      FREE_IMAGES;
      free_filter(filter);
      return -1;
    }

    if (!new_image) {
      fprintf(stderr, "Error: convolution failed\n");
      FREE_IMAGES;
      free_filter(filter);
      return -1;
    }

    const char *new_filename = generate_filename(filename, filter_name);
    if (!new_filename) {
      fprintf(stderr, "Error: failed to generate output filename\n");
      FREE_IMAGES;
      free_filter(filter);
      return -1;
    }

    int ret = store_image(new_filename, new_image);
    if (ret < 0) {
      FREE_IMAGES;
      free_filter(filter);
      return -1;
    }

    FREE_IMAGES;
  }

  free_filter(filter);
  return 0;
}
